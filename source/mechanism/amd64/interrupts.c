#include <arch/amd64/interrupts.h>
#include <return_not.h>

interrupt_entry void isr_dummy_interrupt_entry(struct isr_interrupt_frame* frame)
{
	isr_dummy_interrupt_body(frame);
}

interrupt_body noreturn void isr_dummy_interrupt_body (struct isr_interrupt_frame* frame)
{

	kprintfln
	(
		"--- [CUT HERE] ---\n"
		"isr_dummy: interrupt frame is:\n"
		"IP:\t%X\n"
		"CS:\t%X\n"
		"FLAGS:\t%X\n"
		"SP:\t%X\n"
		"SS:\t%X",
		frame->ip, frame->cs, frame->flags, frame->sp, frame->ss
	);

	panic("uninitialized interrupt service routine called.");

	return_not;
}
