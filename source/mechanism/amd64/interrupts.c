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
