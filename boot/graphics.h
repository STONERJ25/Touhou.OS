#ifndef TOUHOU_GRAPHICS_H
#define TOUHOU_GRAPHICS_H

#include "uefi.h"

/* Text via UEFI's own console (ConOut) -- only usable before ExitBootServices. */
void print_uint(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, UINT32 value);

/* Raw framebuffer drawing -- no UEFI dependency, usable before and after ExitBootServices. */
void put_pixel(UINT8 *framebuffer, UINT32 stride, UINT32 x, UINT32 y, UINT8 r, UINT8 g, UINT8 b);
void fill_rect(UINT8 *framebuffer, UINT32 stride, INT32 x, INT32 y, INT32 width, INT32 height, UINT8 r, UINT8 g, UINT8 b);
void fill_circle(UINT8 *framebuffer, UINT32 stride, INT32 cx, INT32 cy, INT32 radius, UINT8 r, UINT8 g, UINT8 b);

#endif /* TOUHOU_GRAPHICS_H */
