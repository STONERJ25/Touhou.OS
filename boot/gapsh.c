#include "gapsh.h"
#include "console.h"
#include "string.h"
#include "panic.h"
#include "memmap.h"
#include "io.h"
#include "reboot.h"

#define LINE_BUFFER_SIZE 128

/*
 * Commands are typed and matched in uppercase -- not a style choice made
 * here, but a direct consequence of two earlier decisions: the font only
 * has uppercase glyphs, and the keyboard table maps keys straight to
 * uppercase letters (no Shift handling yet).
 */

static void cmd_help(void) {
    console_print("AVAILABLE COMMANDS:\n");
    console_print("  HELP     SHOW THIS LIST\n");
    console_print("  CLEAR    CLEAR THE SCREEN\n");
    console_print("  VERSION  SHOW KERNEL VERSION\n");
    console_print("  ECHO     PRINT BACK WHATEVER FOLLOWS\n");
    console_print("  PANIC    TRIGGER A TEST KERNEL PANIC\n");
    console_print("  MEMMAP   SHOW THE UEFI MEMORY MAP\n");
    console_print("  CPUINFO  SHOW CPU VENDOR AND SIGNATURE\n");
    console_print("  REBOOT   RESET THE MACHINE\n");
    console_print("  CRASH    TRIGGER A REAL CPU DIVIDE BY ZERO\n");
}

static void cmd_version(void) {
    console_print("TOUHOUOS GENSOKYO KERNEL 0.0.1\n");
}

static void cmd_crash(void) {
    console_print("TRIGGERING A REAL DIVIDE BY ZERO...\n");
    volatile int zero = 0; /* volatile -- stops the compiler folding this into a compile-time error */
    int result = 42 / zero;
    console_print_uint((UINT32)result); /* never reached */
}

/* Unpacks a little-endian 32-bit register into 4 ASCII characters --
 * the least significant byte is the FIRST character, not the last. */
static void extract_bytes(char *dest, UINT32 value) {
    dest[0] = (char)(value & 0xFF);
    dest[1] = (char)((value >> 8) & 0xFF);
    dest[2] = (char)((value >> 16) & 0xFF);
    dest[3] = (char)((value >> 24) & 0xFF);
}

static void cmd_cpuinfo(void) {
    UINT32 eax, ebx, ecx, edx;
    char vendor[13];

    cpuid(0, 0, &eax, &ebx, &ecx, &edx);

    /* Vendor string register order is EBX, EDX, ECX -- not alphabetical. */
    extract_bytes(&vendor[0], ebx);
    extract_bytes(&vendor[4], edx);
    extract_bytes(&vendor[8], ecx);
    vendor[12] = 0;
    str_to_upper(vendor); /* CPUID returns mixed case; the font only has uppercase */

    console_print("VENDOR: ");
    console_print(vendor);
    console_print("\nMAX CPUID LEAF: ");
    console_print_uint(eax);

    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    console_print("\nSIGNATURE HEX: ");
    console_print_hex(eax);
    console_print("\n");
}

void gapsh_run(void) {
    char line[LINE_BUFFER_SIZE];

    console_print("\nGAPSH READY. TYPE HELP FOR COMMANDS.\n");

    for (;;) {
        console_print("GAPSH> ");
        console_read_line(line, LINE_BUFFER_SIZE);

        if (str_equals(line, "")) {
            /* empty line -- just show the prompt again */
        } else if (str_equals(line, "HELP")) {
            cmd_help();
        } else if (str_equals(line, "CLEAR")) {
            console_clear();
        } else if (str_equals(line, "VERSION")) {
            cmd_version();
        } else if (str_starts_with(line, "ECHO ")) {
            console_print(line + 5);
            console_print("\n");
        } else if (str_equals(line, "PANIC")) {
            panic("MANUALLY TRIGGERED FROM GAPSH.");
        } else if (str_equals(line, "MEMMAP")) {
            memmap_print();
        } else if (str_equals(line, "CPUINFO")) {
            cmd_cpuinfo();
        } else if (str_equals(line, "REBOOT")) {
            console_print("REBOOTING...\n");
            reboot_now();
        } else if (str_equals(line, "CRASH")) {
            cmd_crash();
        } else {
            console_print("UNKNOWN COMMAND: ");
            console_print(line);
            console_print("\n");
        }
    }
}
