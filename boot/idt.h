#ifndef TOUHOU_IDT_H
#define TOUHOU_IDT_H

/* Sets up the IDT with whatever exception handlers exist so far (currently
 * just divide-by-zero) and loads it via LIDT. Must run after the console is
 * initialized (handlers panic, which prints) and before anything that might
 * trigger a handled exception. Unhandled vectors still triple-fault QEMU
 * (silent reset) -- a known, expected gap for this first pass. */
void idt_init(void);

#endif /* TOUHOU_IDT_H */
