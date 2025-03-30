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

#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_

#define membarrier() \
	__asm__ volatile ("" ::: "memory"); \
	__sync_synchronize();

#include <ext_stdint.h>
#include <buddy.h>
#include <slab.h>

#define PTRERR(ptr) (ptr == NULL || ((intptr_t) ptr < 0))

#define KLARGE_ALLOC_MAGIC 0xB150B150B150B150

void* kmalloc(size_t bytes);
/*
 * void* kmalloc(size_t bytes);
 *
 * Allocate `bytes` bytes with a generic allocator. Memory yielded by this
 * allocator must only be used by the kernel. The actual given amount might
 * be larger.
 *
 * On error, it might return either NULL or ENOMEM. You can check with
 * the PTRERR() macro.
 */

void  kfree  (void*  what);
/*
 * void kfree(void* what);
 *
 * Frees memory allocated by kmalloc().
 */

struct klarge_alloc_header
{
	uint64_t magic;
	size_t   pages;
};

void* klarge_alloc(size_t bytes);
void  klarge_free (void*  what);

#endif // KERNEL_MEMORY_H_
