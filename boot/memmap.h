#ifndef TOUHOU_MEMMAP_H
#define TOUHOU_MEMMAP_H

#include "uefi.h"

/* Called once, right after a successful ExitBootServices, to remember the
 * memory map for later inspection (e.g. gapsh's MEMMAP command). */
void memmap_store(EFI_MEMORY_DESCRIPTOR *buffer, UINTN size, UINTN descriptorSize);

void memmap_print(void);

#endif /* TOUHOU_MEMMAP_H */
