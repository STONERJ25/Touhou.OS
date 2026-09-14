#include "console.h"
#include "graphics.h"
#include "font.h"

#define CONSOLE_SCALE 8                    /* smaller than earlier demos, to fit more text on screen */
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
              (INT32)g_screenWidth, (INT32)LINE_HEIGHT, 0, 0, 0);
}

static void console_newline(void) {
    g_cursorCol = 0;
    g_cursorRow++;

    if (g_cursorRow >= g_rows) {
        console_scroll();
        g_cursorRow = g_rows - 1; /* stay on the last row -- scrolling just made room there */
    }
}

void console_putchar(char c) {
    if (c == '\n') {
        console_newline();
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
