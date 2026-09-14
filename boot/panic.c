#include "panic.h"
#include "console.h"

__attribute__((noreturn)) void panic(const char *message) {
    console_set_background(120, 0, 0); /* panic red */
    console_clear();
    console_set_color(255, 255, 255);  /* white text */

    console_print("\n  KERNEL PANIC\n\n  ");
    console_print(message);
    console_print("\n\n  SYSTEM HALTED.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
