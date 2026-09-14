#ifndef TOUHOU_IO_H
#define TOUHOU_IO_H

#include "uefi.h"

/*
 * x86 has a second address space besides ordinary memory: I/O ports,
 * reachable only through these two instructions, not through pointers.
 * Several classic PC devices (serial ports, the PS/2 keyboard controller)
 * are controlled this way rather than through memory-mapped registers
 * like the framebuffer.
 */

static inline void outb(UINT16 port, UINT8 value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline UINT8 inb(UINT16 port) {
    UINT8 result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

#endif /* TOUHOU_IO_H */
