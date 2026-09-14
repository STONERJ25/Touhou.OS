#include "uefi.h"

static void print_uint(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, UINT32 value) {
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

static void put_pixel(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, UINT32 x, UINT32 y, UINT8 r, UINT8 g, UINT8 b) {
    UINT32 stride = gop->Mode->Info->PixelsPerScanLine;
    UINT8 *pixel  = (UINT8 *)(UINTN)gop->Mode->FrameBufferBase + (y * stride + x) * 4;

    pixel[0] = b; /* confirmed BGR order: byte 0 is blue, byte 2 is red */
    pixel[1] = g;
    pixel[2] = r;
    pixel[3] = 0; /* reserved byte, unused */
}

static void fill_circle(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, INT32 cx, INT32 cy, INT32 radius, UINT8 r, UINT8 g, UINT8 b) {
    /* Assumes the circle stays fully on-screen -- no bounds checking against
     * the real resolution, since we're choosing cx/cy/radius ourselves below. */
    for (INT32 y = cy - radius; y <= cy + radius; y++) {
        for (INT32 x = cx - radius; x <= cx + radius; x++) {
            INT32 dx = x - cx;
            INT32 dy = y - cy;

            if (dx * dx + dy * dy <= radius * radius) {
                put_pixel(gop, (UINT32)x, (UINT32)y, r, g, b);
            }
        }
    }
}

static void fill_rect(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, INT32 x, INT32 y, INT32 width, INT32 height, UINT8 r, UINT8 g, UINT8 b) {
    for (INT32 row = y; row < y + height; row++) {
        for (INT32 col = x; col < x + width; col++) {
            put_pixel(gop, (UINT32)col, (UINT32)row, r, g, b);
        }
    }
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; /* required by the UEFI entry-point signature, unused for now */

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
        (CHAR16 *)L"TouhouOS\r\nGensokyo Kernel 0.0.1\r\n\r\nWelcome to Gensokyo.\r\n");

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = 0;

    EFI_STATUS status = SystemTable->BootServices->LocateProtocol(&gopGuid, 0, (void **)&gop);

    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            (CHAR16 *)L"\r\nFailed to locate Graphics Output Protocol.\r\n");
    } else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"\r\nResolution: ");
        print_uint(SystemTable->ConOut, gop->Mode->Info->HorizontalResolution);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L" x ");
        print_uint(SystemTable->ConOut, gop->Mode->Info->VerticalResolution);

        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            (CHAR16 *)L"\r\nPixel format (0=RGB, 1=BGR, 2=custom mask, 3=BLT-only): ");
        print_uint(SystemTable->ConOut, gop->Mode->Info->PixelFormat);

        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"\r\nPixels per scan line: ");
        print_uint(SystemTable->ConOut, gop->Mode->Info->PixelsPerScanLine);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"\r\n");

        fill_circle(gop, 640, 420, 180, 220, 20, 20); /* apple body */
        fill_rect(gop, 632, 200, 16, 60, 90, 50, 20);  /* stem -- bottom overlaps into the body on purpose */
        fill_circle(gop, 668, 210, 22, 30, 130, 30);   /* leaf -- same primitive as the body, just small and green */
    }

    for (;;) {
        /* Hang here on purpose: returning would hand control back to UEFI's
         * own boot menu, which would likely clear what we just printed. */
    }

    return EFI_SUCCESS;
}
