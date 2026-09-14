#include "console.h"
#include "graphics.h"
#include "font.h"
#include "keyboard.h"

#define CONSOLE_SCALE 4                    /* half the earlier scale -- roughly 4x more character cells */
#define CHAR_ADVANCE  (9 * CONSOLE_SCALE)   /* 8 glyph columns + 1 gap column, scaled up */
#define LINE_HEIGHT   (8 * CONSOLE_SCALE)   /* 8 glyph rows, scaled up */

/*
 * Global console state. This is fine precisely because nothing concurrent
 * exists yet -- no interrupts, no tasks -- so there is only ever one thing
 * touching this state at a time. That assumption stops holding once Phase 5
 * (scheduling) exists, at which point this would need real synchronization.
 */
static UINT8  *g_framebuffer;
static UINT32  g_stride;
static UINT32  g_screenWidth;
static UINT32  g_screenHeight;
static UINT32  g_columns;
static UINT32  g_rows;
static UINT32  g_cursorCol;
static UINT32  g_cursorRow;
static UINT8   g_r = 255, g_g = 255, g_b = 255; /* default: white text */
static UINT8   g_bgR = 0, g_bgG = 0, g_bgB = 0;  /* default: black background */

void console_init(UINT8 *framebuffer, UINT32 stride, UINT32 screenWidth, UINT32 screenHeight) {
    g_framebuffer  = framebuffer;
    g_stride       = stride;
    g_screenWidth  = screenWidth;
    g_screenHeight = screenHeight;
    g_columns      = screenWidth / CHAR_ADVANCE;
    g_rows         = screenHeight / LINE_HEIGHT;
    g_cursorCol    = 0;
    g_cursorRow    = 0;
}

void console_set_color(UINT8 r, UINT8 g, UINT8 b) {
    g_r = r;
    g_g = g;
    g_b = b;
}

void console_set_background(UINT8 r, UINT8 g, UINT8 b) {
    g_bgR = r;
    g_bgG = g;
    g_bgB = b;
}

void console_clear(void) {
    fill_rect(g_framebuffer, g_stride, 0, 0, (INT32)g_screenWidth, (INT32)g_screenHeight, g_bgR, g_bgG, g_bgB);
    g_cursorCol = 0;
    g_cursorRow = 0;
}

static void console_scroll(void) {
    UINT32 lineBytes   = LINE_HEIGHT * g_stride * 4;
    UINT32 screenBytes = g_screenHeight * g_stride * 4;

    /* Shift everything up by one line's worth of rows. Every destination
     * byte here sits at a lower address than its source byte, so copying
     * forward (low addresses to high) always reads a byte before anything
     * later in the loop could overwrite that same position. */
    for (UINT32 i = 0; i < screenBytes - lineBytes; i++) {
        g_framebuffer[i] = g_framebuffer[i + lineBytes];
    }

    /* Clear the newly-exposed last line so old pixels don't linger there. */
    fill_rect(g_framebuffer, g_stride, 0, (INT32)(g_screenHeight - LINE_HEIGHT),
              (INT32)g_screenWidth, (INT32)LINE_HEIGHT, g_bgR, g_bgG, g_bgB);
}

static void console_newline(void) {
    g_cursorCol = 0;
    g_cursorRow++;

    if (g_cursorRow >= g_rows) {
        console_scroll();
        g_cursorRow = g_rows - 1; /* stay on the last row -- scrolling just made room there */
    }
}

static void console_backspace(void) {
    /* Only erases within the current line -- at column 0 this does nothing,
     * rather than reaching back into the previous line (which would need
     * tracking how long that line was, which nothing does yet). */
    if (g_cursorCol == 0) {
        return;
    }

    g_cursorCol--;

    INT32 x = (INT32)(g_cursorCol * CHAR_ADVANCE);
    INT32 y = (INT32)(g_cursorRow * LINE_HEIGHT);
    fill_rect(g_framebuffer, g_stride, x, y, CHAR_ADVANCE, LINE_HEIGHT, g_bgR, g_bgG, g_bgB);
}

void console_putchar(char c) {
    if (c == '\n') {
        console_newline();
        return;
    }

    if (c == '\b') {
        console_backspace();
        return;
    }

    INT32 x = (INT32)(g_cursorCol * CHAR_ADVANCE);
    INT32 y = (INT32)(g_cursorRow * LINE_HEIGHT);

    draw_char(g_framebuffer, g_stride, x, y, CONSOLE_SCALE, c, g_r, g_g, g_b);

    g_cursorCol++;
    if (g_cursorCol >= g_columns) {
        console_newline();
    }
}

void console_print(const char *text) {
    while (*text != 0) {
        if (*text == ' ' || *text == '\n') {
            console_putchar(*text);
            text++;
            continue;
        }

        /* Measure the upcoming word before printing any of it. */
        UINTN wordLength = 0;
        while (text[wordLength] != 0 && text[wordLength] != ' ' && text[wordLength] != '\n') {
            wordLength++;
        }

        /* If the whole word won't fit in what's left of this line, wrap
         * first -- so a word only ever gets split if it's longer than an
         * entire line by itself, not just because it started too late. */
        if (g_cursorCol != 0 && g_cursorCol + wordLength > g_columns) {
            console_newline();
        }

        for (UINTN i = 0; i < wordLength; i++) {
            console_putchar(text[i]);
        }

        text += wordLength;
    }
}

void console_print_uint(UINT64 value) {
    char buffer[21]; /* UINT64 max is 20 digits, plus a null terminator */
    int i = 20;
    buffer[20] = 0;

    if (value == 0) {
        buffer[--i] = '0';
    } else {
        while (value > 0) {
            buffer[--i] = (char)('0' + (value % 10));
            value /= 10;
        }
    }

    console_print(&buffer[i]);
}

void console_print_hex(UINT64 value) {
    static const char *digits = "0123456789ABCDEF";
    char buffer[17]; /* 16 hex digits for a UINT64, plus a null terminator */
    int i = 16;
    buffer[16] = 0;

    if (value == 0) {
        buffer[--i] = '0';
    } else {
        while (value > 0) {
            buffer[--i] = digits[value % 16];
            value /= 16;
        }
    }

    console_print(&buffer[i]);
}

void console_read_line(char *buffer, UINTN maxLength) {
    UINTN length = 0;

    for (;;) {
        char c = keyboard_read_char();

        if (c == '\n') {
            console_putchar('\n');
            break;
        }

        if (c == '\b') {
            if (length > 0) {
                length--;
                console_putchar('\b');
            }
            continue; /* nothing to erase if the line is already empty */
        }

        if (length + 1 < maxLength) { /* leave room for the null terminator */
            buffer[length] = c;
            length++;
            console_putchar(c);
        }
        /* else: buffer full -- silently ignore further characters until Enter/backspace */
    }

    buffer[length] = 0;
}
