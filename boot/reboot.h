#ifndef TOUHOU_REBOOT_H
#define TOUHOU_REBOOT_H

#include "uefi.h"

/* Called once after ExitBootServices succeeds, to remember RuntimeServices --
 * the one part of EFI_SYSTEM_TABLE guaranteed to keep working afterward. */
void reboot_init(EFI_RUNTIME_SERVICES *runtimeServices);

/* Resets the machine. Does not return if it succeeds. */
void reboot_now(void);

#endif /* TOUHOU_REBOOT_H */
