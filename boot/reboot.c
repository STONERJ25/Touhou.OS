#include "reboot.h"

static EFI_RUNTIME_SERVICES *g_runtimeServices;

void reboot_init(EFI_RUNTIME_SERVICES *runtimeServices) {
    g_runtimeServices = runtimeServices;
}

void reboot_now(void) {
    g_runtimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, 0);

    /* Should never reach here -- ResetSystem doesn't return on success. */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
