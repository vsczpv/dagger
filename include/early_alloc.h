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

#ifndef KERNEL_EARLY_ALLOC_H_
#define KERNEL_EARLY_ALLOC_H_

#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>

#define EARLY_ALLOC_BUDGET 2*KiB

/*
 * This is the early boot bump alocator that pools from a small, fixed size array.
 * Some heap functionality is needed even at this early bootstage.
 *
 * Granularity is 1 byte, freeing is not possible.
 */

extern uint8_t early_alloc_memory_pool[EARLY_ALLOC_BUDGET];
extern size_t  early_alloc_used;

void*  early_alloc(size_t size);
size_t early_alloc_get_remaining(void);

#endif // KERNEL_EARLY_ALLOC_H_

