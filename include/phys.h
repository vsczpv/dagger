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

#ifndef KERNEL_PHYS_H_
#define KERNEL_PHYS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * The type of an physmap.
 *
 * Mostly mimics Limine's way of catergorizing things.
 */
enum physmap_type
{
	pm_Usable                 = 0,    /* Usable   chunk of memory */
	pm_Reserved               = 1,    /* Unusable chunk of memory */
	pm_ACPI_Reclaimable       = 2,    /* Reclaimable memory, in current use by ACPI */
	pm_ACPI_NVS               = 3,    /* ACPI reserved memory */
	pm_Bad_Memory             = 4,    /* Unusable chunk of memory, scary edition */
	pm_Bootloader_Reclaimable = 5,    /* Reclaimable memory, in current use by the bootloader */
	pm_Kernel                 = 6,    /* The kernel. This catergory exists due to bootloader shenanigans */
	pm_Framebuffer            = 7     /* Graphical framebuffer given by bootloader */
};

/*
 * This struct represents a chunk of physical memory.
 *
 * - base:   Base address of map
 * - length: How large the chunk is (in bytes)
 * - type:   It's type
 */
struct physmap
{
	void*             base;
	size_t            length;
	enum physmap_type type;
	bool              reaped;
};

/*
 * This singleton struct represents the physical memory of the system
 */
extern struct physmap_descriptor_table
{
	size_t capacity;
	size_t map_count;
	struct physmap* maps;
} physmap_descriptor;

extern void* early_boot_hhdm;
void pm_set_hhdm(void* hhdm);

void pm_allocate_table(unsigned int count);
void pm_add_map       (void* base, size_t length, enum physmap_type type);

struct physmap* pm_find_physmap_with_atleast(size_t length);
struct physmap* pm_find_physmap_highest_usable(void);

/* Was supposed to befined in the linker script and later backed using vm
 * but apparently the linker hates when you give it an address that's farther
 * from pc than 2GiB. */
#define PHYSFRAME_STACK_MAX_ENTRIES (17179869184)
#define PHYSFRAME_STACK_LOCATION    ((intptr_t) 0xffff'f000'0000'0000)
#define PHYSFRAME_STACK_GETPTR() ((intptr_t*) PHYSFRAME_STACK_LOCATION)

//extern intptr_t* (physframe_stack[PHYSFRAME_STACK_MAX_ENTRIES]);

#endif // KERNEL_PHYS_H

