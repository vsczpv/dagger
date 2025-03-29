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

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdnoreturn.h>
#include <serial.h>
#include <stddef.h>

#define KiB (1024)
#define MiB (1024*1024)
#define GiB (1024*1024*1024)

#define PAGE_SIZE (4*KiB)

#define KERNEL_MAX_SIZE 2*MiB

extern void* kernel_physical_base;
extern void* kernel_virtual_base;

extern struct serial_interface kernel_main_serial_port;

extern size_t kernel_size;

/*
 * total_physical_memory represents the amount of memory present in the system.
 * Once initialized, it never changes.
 *
 * available_physical_memory show how much memory the physmgr can mandate.
 *
 * used_physical_memory shows how much memory the physmgr has leased.
 *
 * Used memory that is acquired via something that isn't physmgr is directly
 * subtracted from available_physical_memory; otherwise it is summed to
 * used_physical_memory.
 *
 * To see how much memory the system is using, you calculate:
 *
 *     (total_physical_memory - available_physical_memory) + used_physical_memory
 *
 */
extern size_t total_physical_memory;
extern size_t available_physical_memory;
extern size_t used_physical_memory;

#define HHDM_LOCATION ((intptr_t)  0xffff'9000'0000'0000)
#define HHDM_GETPTR() ((void*)     HHDM_LOCATION)

#define HHDM_VIRT_TO_PHYS(addr) ((intptr_t) (addr) - HHDM_LOCATION)
#define HHDM_PHYS_TO_VIRT(addr) ((void*)   ((addr) + HHDM_LOCATION))

#define CANONICAL(addr) ((((intptr_t) addr) & 0xffff'0000'0000'0000) == 0 || ((intptr_t) addr & 0xffff'0000'0000'0000) == 0xffff'0000'0000'0000)
#define ALIGNED(addr)  ((((intptr_t) addr) & 0xfff) == 0)

#define BITLOG2(x) (32 - __builtin_clz(x) - 1)

#define PAGE_COUNT_FOR_BYTES(x) ((((x - 1) & (~0xfff)) >> 12) + 1)

inline static size_t next_power_of_2(size_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

noreturn void kernel_main(void);

#endif // KERNEL_KERNEL_H_
