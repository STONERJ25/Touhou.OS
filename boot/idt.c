#include "idt.h"
#include "uefi.h"
#include "panic.h"

#define IDT_ENTRY_COUNT 256
#define GATE_TYPE_INTERRUPT_64 0x8E /* present(1) DPL(00) 0 type(1110 = 64-bit interrupt gate) */

/*
 * One IDT entry -- a hardware-defined 16-byte layout, not something we get
 * to design. The handler address is split across three separate fields
 * (low 16 / mid 16 / high 32 bits), which is why installing a handler
 * means slicing its pointer into three pieces below.
 */
typedef struct {
    UINT16 OffsetLow;
    UINT16 Selector;
    UINT8  IST;
    UINT8  TypeAttr;
    UINT16 OffsetMid;
    UINT32 OffsetHigh;
    UINT32 Reserved;
} __attribute__((packed)) IdtEntry;

typedef struct {
    UINT16 Limit;
    UINT64 Base;
} __attribute__((packed)) IdtPointer;

/* Static storage duration -- guaranteed zero-initialized by C itself, so
 * every entry's Present bit (top bit of TypeAttr) already reads as 0,
 * i.e. "not present," before we explicitly fill any entry in. */
static IdtEntry g_idt[IDT_ENTRY_COUNT];

struct InterruptFrame {
    UINT64 InstructionPointer;
    UINT64 CodeSegment;
    UINT64 Flags;
    UINT64 StackPointer;
    UINT64 StackSegment;
};

/* __attribute__((interrupt)) makes the compiler generate the correct
 * hardware-mandated entry/exit code (iretq, stack handling) instead of
 * requiring a hand-written assembly stub. */
__attribute__((interrupt)) static void divide_by_zero_handler(struct InterruptFrame *frame) {
    (void)frame; /* unused for now -- printing the faulting address is a natural later addition */

    /* Clang warns here (-Wexcessive-regsave): calling an ordinary function
     * from an interrupt handler can be unsafe, because the compiler's
     * auto-generated epilogue might not restore every register correctly
     * when it returns to the interrupted code. That doesn't apply here --
     * panic() is noreturn and genuinely never returns (it halts forever),
     * so the epilogue this warning is about never actually executes. */
    panic("DIVIDE BY ZERO EXCEPTION.");
}

static void idt_set_gate(int vector, void *handler, UINT16 selector) {
    UINT64 address = (UINT64)handler;

    g_idt[vector].OffsetLow  = (UINT16)(address & 0xFFFF);
    g_idt[vector].Selector   = selector;
    g_idt[vector].IST        = 0;
    g_idt[vector].TypeAttr   = GATE_TYPE_INTERRUPT_64;
    g_idt[vector].OffsetMid  = (UINT16)((address >> 16) & 0xFFFF);
    g_idt[vector].OffsetHigh = (UINT32)((address >> 32) & 0xFFFFFFFF);
    g_idt[vector].Reserved   = 0;
}

void idt_init(void) {
    /* Reuse whatever code segment UEFI already left active, rather than
     * building our own GDT -- a separate, later step. */
    UINT16 currentCodeSegment;
    __asm__ volatile ("mov %%cs, %0" : "=r"(currentCodeSegment));

    idt_set_gate(0, (void *)divide_by_zero_handler, currentCodeSegment);

    IdtPointer idtPointer;
    idtPointer.Limit = sizeof(g_idt) - 1;
    idtPointer.Base  = (UINT64)&g_idt[0];

    __asm__ volatile ("lidt %0" : : "m"(idtPointer));
}
