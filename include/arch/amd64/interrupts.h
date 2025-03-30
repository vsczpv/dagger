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
 * From GCC manual
 * https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
 */
#ifdef __x86_64__
typedef unsigned long long int uword_t;
#else
typedef unsigned int uword_t;
#endif

/*
 * We use the same GCC extension to write the ISRs directly in C.
 * Note that, because an __attribute__ ((interrupt)) function has different register
 * backup semantics than sysv, it is necessary to call a separate "body" function so
 * that we can call into other kernel functions.
 */
#define interrupt_entry __attribute__ ((interrupt,no_stack_protector))
#define interrupt_body  __attribute__ ((no_caller_saved_registers,no_stack_protector))

interrupt_entry          void isr_dummy_interrupt_entry(struct isr_interrupt_frame* frame);
interrupt_body  noreturn void isr_dummy_interrupt_body (struct isr_interrupt_frame* frame);

interrupt_entry          void isr_double_fault_entry(struct isr_interrupt_frame* frame, uword_t arg);
interrupt_body  noreturn void isr_double_fault_body (struct isr_interrupt_frame* frame, uword_t arg);

interrupt_entry          void isr_page_fault_entry(struct isr_interrupt_frame* frame, uword_t arg);
interrupt_body  noreturn void isr_page_fault_body (struct isr_interrupt_frame* frame, uword_t arg);

interrupt_entry          void isr_gpf_entry(struct isr_interrupt_frame* frame, uword_t arg);
interrupt_body  noreturn void isr_gpf_body (struct isr_interrupt_frame* frame, uword_t arg);

/*
 * Interrupt Definitions
 */
#define X86_ISR_DOUBLEFAULT 0x08
#define X86_ISR_PAGEFAULT   0x0e
#define X86_ISR_GPF         0x0d

#endif // KERNEL_ARCH_AMD64_INTERRUPTS_H_
