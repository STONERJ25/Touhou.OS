#ifndef TOUHOU_PANIC_H
#define TOUHOU_PANIC_H

/* Prints message on a distinct full-screen background and halts forever.
 * Requires the console to already be initialized. */
__attribute__((noreturn)) void panic(const char *message);

#endif /* TOUHOU_PANIC_H */
