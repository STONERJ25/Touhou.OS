#ifndef TOUHOU_CONSOLE_H
#define TOUHOU_CONSOLE_H

#include "uefi.h"

/* Must be called once before any other console_* function. */
void console_init(UINT8 *framebuffer, UINT32 stride, UINT32 screenWidth, UINT32 screenHeight);

void console_set_color(UINT8 r, UINT8 g, UINT8 b);
void console_putchar(char c);
void console_print(const char *text);

#endif /* TOUHOU_CONSOLE_H */
