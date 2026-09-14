#include "uefi.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; /* required by the UEFI entry-point signature, unused for now */

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
        (CHAR16 *)L"TouhouOS\r\nGensokyo Kernel 0.0.1\r\n\r\nWelcome to Gensokyo.\r\n");

    for (;;) {
        /* Hang here on purpose: returning would hand control back to UEFI's
         * own boot menu, which would likely clear what we just printed. */
    }

    return EFI_SUCCESS;
}
