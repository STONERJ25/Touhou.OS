#include "keyboard.h"
#include "io.h"
#include "uefi.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64
#define OUTPUT_BUFFER_FULL   0x01 /* status bit 0: a byte is waiting at the data port */

typedef struct {
    UINT8 scancode;
    char  character;
} ScancodeEntry;

/*
 * Standard PS/2 Scancode Set 1 "make codes" (sent on key press; a release
 * sends the same code with bit 7 set). A well-documented, standard mapping,
 * not derived here -- letters/digits increase left-to-right across each
 * physical keyboard row, which is why the values aren't alphabetical.
 */
static const ScancodeEntry SCANCODE_TABLE[] = {
    { 0x02, '1' }, { 0x03, '2' }, { 0x04, '3' }, { 0x05, '4' }, { 0x06, '5' },
    { 0x07, '6' }, { 0x08, '7' }, { 0x09, '8' }, { 0x0A, '9' }, { 0x0B, '0' },
    { 0x10, 'Q' }, { 0x11, 'W' }, { 0x12, 'E' }, { 0x13, 'R' }, { 0x14, 'T' },
    { 0x15, 'Y' }, { 0x16, 'U' }, { 0x17, 'I' }, { 0x18, 'O' }, { 0x19, 'P' },
    { 0x1E, 'A' }, { 0x1F, 'S' }, { 0x20, 'D' }, { 0x21, 'F' }, { 0x22, 'G' },
    { 0x23, 'H' }, { 0x24, 'J' }, { 0x25, 'K' }, { 0x26, 'L' },
    { 0x2C, 'Z' }, { 0x2D, 'X' }, { 0x2E, 'C' }, { 0x2F, 'V' }, { 0x30, 'B' },
    { 0x31, 'N' }, { 0x32, 'M' },
    { 0x39, ' '  }, /* space */
    { 0x1C, '\n' }, /* enter */
    { 0x0E, '\b' }, /* backspace */
};
#define SCANCODE_TABLE_COUNT (sizeof(SCANCODE_TABLE) / sizeof(SCANCODE_TABLE[0]))

static char scancode_to_char(UINT8 scancode) {
    for (UINTN i = 0; i < SCANCODE_TABLE_COUNT; i++) {
        if (SCANCODE_TABLE[i].scancode == scancode) {
            return SCANCODE_TABLE[i].character;
        }
    }
    return 0; /* no mapping for this key yet (arrows, Shift, function keys, ...) */
}

char keyboard_read_char(void) {
    for (;;) {
        if (inb(KEYBOARD_STATUS_PORT) & OUTPUT_BUFFER_FULL) {
            UINT8 scancode = inb(KEYBOARD_DATA_PORT);

            if ((scancode & 0x80) == 0) { /* bit 7 set means "key released" -- ignore those */
                char c = scancode_to_char(scancode);
                if (c != 0) {
                    return c;
                }
            }
        }
    }
}
