#include "memmap.h"
#include "console.h"

#define EFI_CONVENTIONAL_MEMORY 7 /* "free RAM" -- the type we actually care about most */

static EFI_MEMORY_DESCRIPTOR *g_buffer;
static UINTN                  g_size;
static UINTN                  g_descriptorSize;

void memmap_store(EFI_MEMORY_DESCRIPTOR *buffer, UINTN size, UINTN descriptorSize) {
    g_buffer         = buffer;
    g_size           = size;
    g_descriptorSize = descriptorSize;
}

void memmap_print(void) {
    UINTN entryCount = g_size / g_descriptorSize;

    console_print("MEMORY MAP: ");
    console_print_uint(entryCount);
    console_print(" ENTRIES\n\n");

    /*
     * The real stride between entries is g_descriptorSize (whatever
     * GetMemoryMap actually reported), never sizeof(EFI_MEMORY_DESCRIPTOR)
     * -- the spec allows descriptors to grow in future revisions, so this
     * has to be manual byte-pointer arithmetic, not array indexing.
     */
    UINT8 *entry = (UINT8 *)g_buffer;

    for (UINTN i = 0; i < entryCount; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)entry;

        if (desc->Type == EFI_CONVENTIONAL_MEMORY) {
            console_print("FREE  ");
        } else {
            console_print("TYPE ");
            console_print_uint(desc->Type);
            console_print(" ");
        }

        console_print(" 0x");
        console_print_hex(desc->PhysicalStart);
        console_print("  ");
        console_print_uint(desc->NumberOfPages);
        console_print(" PAGES\n");

        entry += g_descriptorSize;
    }
}
