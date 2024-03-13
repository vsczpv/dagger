#ifndef KERNEL_ARCH_AMD64_INTERRUPTS_H_
#define KERNEL_ARCH_AMD64_INTERRUPTS_H_

#include <stdint.h>
#include <stdnoreturn.h>

/*
 * This struct represents all values pushed to the stack by an interrupt when
 * an ISR gets called. It is populated by a GCC extension.
 */
struct isr_interrupt_frame
{
	uint64_t ip;
	uint64_t cs;
	uint64_t flags;
	uint64_t sp;
	uint64_t ss;
};

/*
 * We use the same GCC extension to write the ISRs directly in C.
 * Note that, because an __attribute__ ((interrupt)) function has different register
 * backup semantics than sysv, it is necessary to call a separate "body" function so
 * that we can call into other kernel functions.
 */
#define interrupt_entry __attribute__ ((interrupt))
#define interrupt_body  __attribute__ ((no_caller_saved_registers))

interrupt_entry          void isr_dummy_interrupt_entry(struct isr_interrupt_frame* frame);
interrupt_body  noreturn void isr_dummy_interrupt_body (struct isr_interrupt_frame* frame);

#endif // KERNEL_ARCH_AMD64_INTERRUPTS_H_
