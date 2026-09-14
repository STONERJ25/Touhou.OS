#include "graphics.h"

void print_uint(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, UINT32 value) {
    CHAR16 buffer[11]; /* UINT32 max is 10 digits, plus a null terminator */
    int i = 10;
    buffer[10] = 0;

    if (value == 0) {
        buffer[--i] = '0';
    } else {
        while (value > 0) {
            buffer[--i] = (CHAR16)('0' + (value % 10));
            value /= 10;
        }
    }

    out->OutputString(out, &buffer[i]);
}

void put_pixel(UINT8 *framebuffer, UINT32 stride, UINT32 x, UINT32 y, UINT8 r, UINT8 g, UINT8 b) {
    UINT8 *pixel = framebuffer + (y * stride + x) * 4;

    pixel[0] = b; /* confirmed BGR order: byte 0 is blue, byte 2 is red */
    pixel[1] = g;
    pixel[2] = r;
    pixel[3] = 0; /* reserved byte, unused */
}

void fill_circle(UINT8 *framebuffer, UINT32 stride, INT32 cx, INT32 cy, INT32 radius, UINT8 r, UINT8 g, UINT8 b) {
    /* Assumes the circle stays fully on-screen -- no bounds checking against
     * the real resolution, since callers choose cx/cy/radius themselves. */
    for (INT32 y = cy - radius; y <= cy + radius; y++) {
        for (INT32 x = cx - radius; x <= cx + radius; x++) {
            INT32 dx = x - cx;
            INT32 dy = y - cy;

            if (dx * dx + dy * dy <= radius * radius) {
                put_pixel(framebuffer, stride, (UINT32)x, (UINT32)y, r, g, b);
            }
        }
    }
}

void fill_rect(UINT8 *framebuffer, UINT32 stride, INT32 x, INT32 y, INT32 width, INT32 height, UINT8 r, UINT8 g, UINT8 b) {
    for (INT32 row = y; row < y + height; row++) {
        for (INT32 col = x; col < x + width; col++) {
            put_pixel(framebuffer, stride, (UINT32)col, (UINT32)row, r, g, b);
        }
    }
}
