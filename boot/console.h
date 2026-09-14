#ifndef TOUHOU_CONSOLE_H
#define TOUHOU_CONSOLE_H

#include "uefi.h"

/* Must be called once before any other console_* function. */
void console_init(UINT8 *framebuffer, UINT32 stride, UINT32 screenWidth, UINT32 screenHeight);

void console_set_color(UINT8 r, UINT8 g, UINT8 b);
void console_set_background(UINT8 r, UINT8 g, UINT8 b);
void console_clear(void);

/* Handles '\n' (new line) and '\b' (backspace, current line only) specially. */
void console_putchar(char c);
void console_print(const char *text);
void console_print_uint(UINT64 value);
void console_print_hex(UINT64 value);

/* Blocks until Enter is pressed, echoing keystrokes (including backspace)
 * as they arrive. Writes at most maxLength-1 characters plus a null
 * terminator into buffer; extra characters beyond that are dropped. */
void console_read_line(char *buffer, UINTN maxLength);

#endif /* TOUHOU_CONSOLE_H */
