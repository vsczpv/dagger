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

#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>
#include <early_alloc.h>
#include <panic.h>

uint8_t early_alloc_memory_pool[EARLY_ALLOC_BUDGET] = {0};
size_t  early_alloc_used                            = 0;

void* early_alloc(size_t size)
{

	void* allc = &early_alloc_memory_pool[early_alloc_used];

	early_alloc_used      += size;

	if (early_alloc_used > EARLY_ALLOC_BUDGET) panic("kernel out of memory (early boot bump allocator).");

	return allc;
}

size_t early_alloc_get_remaining(void)
{
	return EARLY_ALLOC_BUDGET - early_alloc_used;
}
