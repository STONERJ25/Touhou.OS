#include "serial.h"
#include "io.h"
#include "uefi.h"

#define COM1 0x3F8

/*
 * Standard 16550 UART bring-up sequence for COM1 -- this exact recipe
 * (register offsets and magic values) is the well-established, widely
 * documented way to initialize a PC serial port, not something derived
 * from scratch here.
 */
void serial_init(void) {
    outb(COM1 + 1, 0x00); /* disable interrupts -- we poll instead */
    outb(COM1 + 3, 0x80); /* enable DLAB, to set the baud rate divisor */
    outb(COM1 + 0, 0x03); /* divisor low byte: 3 -> 38400 baud */
    outb(COM1 + 1, 0x00); /* divisor high byte */
    outb(COM1 + 3, 0x03); /* disable DLAB; 8 data bits, no parity, 1 stop bit */
    outb(COM1 + 2, 0xC7); /* enable and clear the FIFO buffers */
    outb(COM1 + 4, 0x0B); /* enable RTS/DTR */
}

static int transmit_ready(void) {
    return inb(COM1 + 5) & 0x20; /* bit 5 of the line status register: transmit buffer empty */
}

void serial_write_char(char c) {
    while (!transmit_ready()) {
        /* wait for the port to be ready for another byte */
    }
    outb(COM1, (UINT8)c);
}

void serial_write(const char *text) {
    while (*text != 0) {
        serial_write_char(*text);
        text++;
    }
}
