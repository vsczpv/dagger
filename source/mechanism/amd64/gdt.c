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

#include <arch/amd64/gdt.h>

#include <ktext.h>

/* The global descriptor table */
volatile struct gdt kernel_gdt                    = {0};
volatile struct gdt_location_info kernel_gdt_gdtl = {0};

struct gdt_entry gdt_entry_create(uint32_t base, uint32_t limit, uint8_t flags, uint8_t access_byte)
{

	struct gdt_entry entry = {0};

	entry.limit_lo_part = (limit & GDT_ENTRY_MASK_LIMIT_LO_PART);

	entry.base_lo_part = (base  & GDT_ENTRY_MASK_BASE_LO_PART);
	entry.base_mi_part = (base  & GDT_ENTRY_MASK_BASE_MI_PART) >> GDT_ENTRY_SHIFT_BASE_MI_PART;

	entry.access_byte = access_byte;

	entry.flags_and_limit_hi_part  = (limit & GDT_ENTRY_MASK_LIMIT_HI_PART) >> GDT_ENTRY_SHIFT_LIMIT_HI_PART;
	entry.flags_and_limit_hi_part |= flags << GDT_ENTRY_SHIFT_FLAGS;

	entry.base_hi_part = (base & GDT_ENTRY_MAKS_BASE_HI_PART) >> GDT_ENTRY_SHIFT_BASE_HI_PART;

	return entry;
}

/* Execute the lgdt instruction and set the segment registers */
static void __attribute__ ((naked)) gdt_load_execute_lgdt(uint64_t, uint64_t)
{
	__asm__ volatile
	(
		"   lgdt (kernel_gdt_gdtl)\n"
		"   leaq  1f(%%rip), %%rax\n"		//////
		"   pushq %%rdi\n"
		"   pushq %%rax\n"
		"   lretq\n"
		"1: movw  %%si, %%ax\n"
		"   movw  %%ax, %%ds\n"
		"   movw  %%ax, %%es\n"
		"   movw  %%ax, %%fs\n"
		"   movw  %%ax, %%gs\n"
		"   movw  %%ax, %%ss\n"
		"   movq  0(%%rsp), %%r9\n"
		"   ret"
		:::
	);
}

/* Load the GDT */
void gdt_load(uint64_t cs, uint64_t ds)
{

	kernel_gdt_gdtl.base  = (uint64_t) &kernel_gdt.entries;
	kernel_gdt_gdtl.limit = kernel_gdt.count * sizeof (struct gdt_entry) - 1;

	gdt_load_execute_lgdt(cs, ds);

	return;
}

/* Set current segment */
void __attribute__ ((naked)) gdt_set_segment(uint64_t, uint64_t)
{
	__asm__ volatile
	(
		"   leaq  2f(%%rip), %%rax\n"		//////
		"   pushq %%rdi\n"
		"   pushq %%rax\n"
		"   lretq\n"
		"2: movw  %%si, %%ax\n"
		"   movw  %%ax, %%ds\n"
		"   movw  %%ax, %%es\n"
		"   movw  %%ax, %%fs\n"
		"   movw  %%ax, %%gs\n"
		"   movw  %%ax, %%ss\n"
		"   ret"
		:::
	);
}

/*
 * Populate default GDT and load it.
 * It replaces Limine's default one.
 */
void gdt_init(void)
{

	/* Create null descriptor */
	kernel_gdt.entries[kernel_gdt.count++] = gdt_entry_create
	(
		0, 0, 0, GDT_ENTRY_AB_DISABLED
	);

	/*
	 * Create kernel's code segment
	 * NOTE: Must be at GDT_KERNEL_CS = 0x8
	 */
	kernel_gdt.entries[kernel_gdt.count++] = gdt_entry_create
	(
		0x00000000, 0xffffffff,
		GDT_ENTRY_F_KiB | GDT_ENTRY_F_CS64BIT,
		GDT_ENTRY_AB_PRESENT            |
		GDT_ENTRY_AB_DPL_RING0          |
		GDT_ENTRY_AB_CDSEG              |
		GDT_ENTRY_AB_CODE_SEGMENT       |
		GDT_ENTRY_AB_CS_RING_MUST_MATCH |
		GDT_ENTRY_AB_CS_READABLE        |
		GDT_ENTRY_AB_WAS_ACCESSED
	);

	/*
	 * Create kernel's data segment
	 * NOTE: Must be at GDT_KERNEL_DS = 0x10
	 */
	kernel_gdt.entries[kernel_gdt.count++] = gdt_entry_create
	(
		0x00000000, 0xffffffff,
		GDT_ENTRY_F_KiB | GDT_ENTRY_F_32BIT,
		GDT_ENTRY_AB_PRESENT      |
		GDT_ENTRY_AB_DPL_RING0    |
		GDT_ENTRY_AB_CDSEG        |
		GDT_ENTRY_AB_DATA_SEGMENT |
		GDT_ENTRY_AB_DS_GROW_UP   |
		GDT_ENTRY_AB_DS_WRITABLE  |
		GDT_ENTRY_AB_WAS_ACCESSED
	);

	/* Load it */
	gdt_load(GDT_KERNEL_CS, GDT_KERNEL_DS);

	kprintln("GDT successfully initialized.");

	return;
}
