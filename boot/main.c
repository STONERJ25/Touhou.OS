#include "uefi.h"
#include "graphics.h"
#include "font.h"
#include "console.h"
#include "serial.h"
#include "memmap.h"
#include "reboot.h"
#include "gapsh.h"

/* No heap allocator exists yet, so this is a fixed-size static reservation
 * rather than something sized exactly to fit -- generous for a QEMU VM's
 * memory map (typically well under 100 descriptors). */
static UINT8 memoryMapBuffer[16384];

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
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

        UINT8  *framebuffer = (UINT8 *)(UINTN)gop->Mode->FrameBufferBase;
        UINT32  stride      = gop->Mode->Info->PixelsPerScanLine;
        UINT32  width       = gop->Mode->Info->HorizontalResolution;
        UINT32  height      = gop->Mode->Info->VerticalResolution;

        fill_circle(framebuffer, stride, 640, 420, 180, 220, 20, 20); /* apple body */
        fill_rect(framebuffer, stride, 632, 200, 16, 60, 90, 50, 20); /* stem */
        fill_circle(framebuffer, stride, 668, 210, 22, 30, 130, 30);  /* leaf */

        int   exited        = 0;
        UINTN mapSize       = 0;
        UINTN descriptorSize = 0;

        for (int attempt = 0; attempt < 3; attempt++) {
            UINTN  mapKey            = 0;
            UINT32 descriptorVersion = 0;

            mapSize = sizeof(memoryMapBuffer); /* reset each attempt -- GetMemoryMap shrinks it to the size actually used */

            /* Not checking this call's own status -- trusting the 16KB buffer
             * above is large enough, rather than also handling EFI_BUFFER_TOO_SMALL. */
            SystemTable->BootServices->GetMemoryMap(&mapSize, (EFI_MEMORY_DESCRIPTOR *)memoryMapBuffer, &mapKey,
                                                     &descriptorSize, &descriptorVersion);

            if (SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey) == EFI_SUCCESS) {
                exited = 1;
                break;
            }
            /* MapKey went stale -- something changed memory between GetMemoryMap
             * and here -- so fetch a fresh map and try again. */
        }

        if (exited) {
            /* Boot Services are gone now, permanently, for the rest of this boot --
             * including ConOut. This line makes zero UEFI calls: framebuffer,
             * stride, width, and height are plain numbers already saved above. */
            fill_rect(framebuffer, stride, 0, 0, (INT32)width, (INT32)height, 20, 160, 60);

            memmap_store((EFI_MEMORY_DESCRIPTOR *)memoryMapBuffer, mapSize, descriptorSize);
            reboot_init(SystemTable->RuntimeServices);

            /* Our own console, drawn with zero UEFI calls -- cursor-tracked,
             * multi-line printing instead of one hand-positioned draw_string call. */
            console_init(framebuffer, stride, width, height);
            console_set_color(255, 255, 255);
            console_set_background(20, 160, 60); /* matches the green fill above -- scroll/backspace clear to this now, not black */

            /* A second, independent output path -- no framebuffer, no GOP,
             * just I/O ports. Visible in the terminal, not the QEMU window. */
            serial_init();
            serial_write("TouhouOS: independent of firmware. Serial output online.\r\n");

            console_print("GENSOKYO KERNEL 0.0.1\n");
            console_print("INDEPENDENT OF FIRMWARE\n");
            console_print("TEXT CONSOLE ONLINE\n");
            console_print("* WELCOME TO GENSOKYO *\n");

            /* gapsh's command loop -- never returns. Click the QEMU window
             * first so it has keyboard focus. */
            gapsh_run();
        } else {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                (CHAR16 *)L"\r\nExitBootServices failed after 3 attempts.\r\n");
        }
    }

    for (;;) {
        /* Hang here on purpose: returning would hand control back to UEFI's
         * own boot menu, which would likely clear what we just printed. */
    }

    return EFI_SUCCESS;
}
