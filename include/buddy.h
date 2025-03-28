/*
 * Copyright © 2025 Vinícius Schütz Piva
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

#ifndef KERNEL_BUDDY_H_
#define KERNEL_BUDDY_H_

#include <kernel.h>
#include <paging.h>
#include <ext_stdint.h>

#define VMALLOC_AREA_LOCATION ((intptr_t) 0xffffffff84000000)
#define VMALLOC_AREA_GETPTR() ((void*) VMALLOC_AREA_LOCATION)

#define VMALLOC_AREA_SIZE (256*MiB)
#define VMALLOC_AREA_BUDDY_LIST_SIZE (VMALLOC_AREA_SIZE / (4*KiB) / sizeof (uint8_t))
#define VMALLOC_AREA_BUDDY_LIST_SIZE_BITS (VMALLOC_AREA_BUDDY_LIST_SIZE / sizeof (uint8_t))

#define BUDDY_GRANULARITY_MAX (128*MiB)
#define BUDDY_GRANULARITY_MIN (4*KiB)

#define BITOF(list, i)     ((list[i >> 3] & (1 << (i & 0b111))) >> (i & 0b111))
#define SETBIT(list, i, v) (list[i >> 3] = (list[i >> 3] & ~(1 << (i & 0b111))) | (v << (i & 0b111)))

/*
 * A buddy region is a bitvector that represents some contiguous region. Usually memory.
 */

struct buddy_region
{
	uint8_t* array;
	size_t buddylen;
};

extern uint8_t vmalloc_area_buddy_list[VMALLOC_AREA_BUDDY_LIST_SIZE];

extern struct buddy_region vmalloc_buddy;

ssize_t buddy_alloc(struct buddy_region* buddy, size_t  ct);
/*
 * ssize_t buddy_alloc(struct buddy_region* buddy, size_t ct);
 *
 * Allocates `ct` blocks on `buddy`.
 *
 * Returns the index of the start of the allocated region.
 *
 * Errors: [negative]
 *     -ENOMEM - The buddy region is full; requested memory doesn't fit.
 */

void    buddy_free (struct buddy_region* buddy, ssize_t pos, size_t ct);
/*
 * void buddy_free (struct buddy_region* buddy, ssize_t pos, size_t ct);
 *
 * Frees `[pos; ct]` on `buddy`.
 */

void* vmalloc(size_t bytes);
/*
 * void* vmalloc(size_t bytes);
 *
 * Allocates a virtual address range on the kernel's vmalloc area.
 *
 * Note that while the function takes in bytes, it's granurality is of 4KiB.
 *
 * Returns a negative pointer.
 *
 * Errors:
 *     ENOMEM - The kernel's vmalloc area is full.
 */


void  vfree(void* vaddr, size_t bytes);
/*
 * void vfree(void* vaddr, size_t bytes);
 *
 * Frees the `[vaddr; bytes]` virtual address range on the kernel's vmalloc area.
 *
 */

#endif // KERNEL_BUDDY_H_
