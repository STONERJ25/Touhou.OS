#ifndef TOUHOU_FONT_H
#define TOUHOU_FONT_H

#include "uefi.h"

void draw_char(UINT8 *framebuffer, UINT32 stride, INT32 x, INT32 y, INT32 scale, char c, UINT8 r, UINT8 g, UINT8 b);
void draw_string(UINT8 *framebuffer, UINT32 stride, INT32 x, INT32 y, INT32 scale, const char *text, UINT8 r, UINT8 g, UINT8 b);

#endif /* TOUHOU_FONT_H */
