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

	physmap_descriptor.map_count++;

	if (physmap_descriptor.map_count > physmap_descriptor.capacity) panic("overflow in physmap_descriptor");

	return;
}
