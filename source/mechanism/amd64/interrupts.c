/*
 * Copyright © 2024 Vinícius Schütz Piva
 *
 * This file is part of dagger
 *
 * dagger is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 *
 */

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
		"--8<-- [CUT HERE] --8<--\n"
		"isr_dummy: interrupt frame is:\n"
		"IP:\t%X\n"
		"CS:\t%X\n"
		"FLAGS:\t%X\n"
		"SP:\t%X\n"
		"SS:\t%X\n",
		frame->ip, frame->cs, frame->flags, frame->sp, frame->ss
	);

	panic("uninitialized interrupt service routine called.");

	return_not;
}

interrupt_entry void isr_double_fault_entry(struct isr_interrupt_frame* frame, uword_t arg)
{
	isr_double_fault_body(frame, arg);
}

interrupt_body noreturn void isr_double_fault_body(struct isr_interrupt_frame* frame, uword_t arg)
{

	kprintfln
	(
		"--8<-- [CUT HERE] --8<--\n"
		"isr_double_fault: interrupt frame is:\n"
		"IP:\t%X\n"
		"CS:\t%X\n"
		"FLAGS:\t%X\n"
		"SP:\t%X\n"
		"SS:\t%X\n"
		"CODE:\t%X\n",
		frame->ip, frame->cs, frame->flags, frame->sp, frame->ss, arg
	);

	panic("double fault.");

	return_not;
}

interrupt_entry void isr_page_fault_entry(struct isr_interrupt_frame* frame, uword_t arg)
{
	isr_page_fault_body(frame, arg);
}

interrupt_body noreturn void isr_page_fault_body (struct isr_interrupt_frame* frame, uword_t arg)
{

	kprintfln
	(
		"--8<-- [CUT HERE] --8<--\n"
		"isr_page_fault: interrupt frame is:\n"
		"IP:\t%X\n"
		"CS:\t%X\n"
		"FLAGS:\t%X\n"
		"SP:\t%X\n"
		"SS:\t%X\n"
		"CODE:\t%X\n",
		frame->ip, frame->cs, frame->flags, frame->sp, frame->ss, arg
	);

	panic("unhandled page fault in kernel.");

	return_not;
}

interrupt_entry void isr_gpf_entry(struct isr_interrupt_frame* frame, uword_t arg)
{
	isr_gpf_body(frame, arg);
}

interrupt_body noreturn void isr_gpf_body (struct isr_interrupt_frame* frame, uword_t arg)
{

	kprintfln
	(
		"--8<-- [CUT HERE] --8<--\n"
		"isr_page_fault: interrupt frame is:\n"
		"IP:\t%X\n"
		"CS:\t%X\n"
		"FLAGS:\t%X\n"
		"SP:\t%X\n"
		"SS:\t%X\n"
		"CODE:\t%X\n",
		frame->ip, frame->cs, frame->flags, frame->sp, frame->ss, arg
	);

	panic("unhandled general protection fault in kernel.");

	return_not;
}
