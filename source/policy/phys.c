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

#include <phys.h>
#include <early_alloc.h>
#include <panic.h>
#include <errno.h>

struct physmap_descriptor_table physmap_descriptor =
{
	.capacity  = 0,
	.map_count = 0,
	.maps      = NULL
};

void* early_boot_hhdm;

void pm_set_hhdm(void* hhdm)
{

	early_boot_hhdm = hhdm;

	return;
}

void pm_allocate_table(unsigned int count)
{

	physmap_descriptor.capacity = count;
	physmap_descriptor.maps     = early_alloc(sizeof (struct physmap) * count);

	return;
}

void pm_add_map(void* base, size_t length, enum physmap_type type)
{

	struct physmap* map = &physmap_descriptor.maps[physmap_descriptor.map_count];

	map->base   = base;
	map->length = length;
	map->type   = type;
	map->reaped = false;

	physmap_descriptor.map_count++;

	if (physmap_descriptor.map_count > physmap_descriptor.capacity) panic("overflow in physmap_descriptor");

	return;
}

struct physmap* pm_find_physmap_with_atleast(size_t length)
{

	for (size_t i = 0; i < physmap_descriptor.map_count; i++)
	{
		struct physmap* map = &physmap_descriptor.maps[i];
		if (map->length >= length) {
			return map;
		}
	}

	return NULL;
}

struct physmap* pm_find_physmap_highest_usable(void)
{

	static struct physmap dummy = { .base = 0x0 };
	struct physmap* map = &dummy;

	for (size_t i = 0; i < physmap_descriptor.map_count; i++)
	{
		struct physmap* candidate = &physmap_descriptor.maps[i];
		if (candidate->type == pm_Usable || candidate->type == pm_Bootloader_Reclaimable)
		{
			if (candidate->base > map->base)
				map = candidate;
		}
	}

	if (map == &dummy)
		return NULL;
	else
		return map;
}


size_t pm_pfstack_current_pos = 0;

int pm_push_frame(intptr_t phys)
{

	if ((phys & 0xfff) != 0)
		return EFAULT;

	if (((pm_pfstack_current_pos + 1) * 4*KiB) >= available_physical_memory)
		return EDOM;

	PHYSFRAME_STACK_GETPTR()[pm_pfstack_current_pos++] = phys;
	used_physical_memory -= 4*KiB;

	return 0;
}

intptr_t pm_pop_frame(void)
{
	if (used_physical_memory == available_physical_memory)
		return (intptr_t) NULL;

	used_physical_memory += 4*KiB;

	return PHYSFRAME_STACK_GETPTR()[--pm_pfstack_current_pos];
}
