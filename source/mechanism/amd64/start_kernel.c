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

#include <stdnoreturn.h>

#include <arch/amd64/limine.h>
#include <arch/amd64/gdt.h>
#include <arch/amd64/idt.h>
#include <arch/amd64/com0.h>
#include <arch/amd64/pagetables.h>

#include <vt_escape_sequences.h>
#include <early_alloc.h>
#include <phys.h>
#include <panic.h>
#include <stddef.h>
#include <ktext.h>

#include <kernel.h>

/* Selecting the bootloader's revision */
LIMINE_BASE_REVISION(1)

/*
 * This struct queries boot memory map information
 */
volatile struct limine_memmap_request memmap_request =
{
	.id       = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

/*
 * This struct queries limine for the higher half direct map address
 */
volatile struct limine_hhdm_request hhdm_request =
{
	.id       = LIMINE_HHDM_REQUEST,
	.revision = 0,
};

/*
 * This struct queries limine for the kernel's address
 */
volatile struct limine_kernel_address_request kaddr_request =
{
	.id       = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0,
};

/*
 * This function takes the memmap info from limine and properly loads it into
 * the kernel.
 */
static void limine_scan_boot_memmaps(void)
{

	/* The kernel cannot continue booting without knowning it's own RAM */
	if (memmap_request.response == NULL) panic("no memmap request answered.");

	/* Neither can the kernel continue without knowing where the HHDM is */
	if (hhdm_request.response == NULL) panic("no HHDM request answered.");

	struct limine_memmap_response* memmap = memmap_request.response;
	struct limine_hhdm_response*   hhdm   = hhdm_request  .response;

	kprintfln("limine: HHDM virtual address: %X", hhdm->offset);

	kprintfln("limine: memory maps: %i", memmap->entry_count);

	for (size_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry* e = memmap->entries[i];

		/* Print this information */

		size_t length = e->length;
		char*  unit   = "bytes";

		if (e->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
		{
			if (length >= KERNEL_MAX_SIZE)
				panic("kernel too large");
			kernel_size = length;
		}

		else if (e->type == LIMINE_MEMMAP_USABLE || e->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
		{
			available_physical_memory += e->length;
		}

		static char* types[] = {"usable", "reserved", "acpi reclaimable", "acpi nvs", "bad memory", "bootloader reclaimable", "kernel", "framebuffer"};
		char* type    = types[e->type];

		if      (length >= GiB) unit = "GiB", length /=  GiB;
		else if (length >= MiB) unit = "MiB", length /=  MiB;
		else if (length >= KiB) unit = "KiB", length /=  KiB;

		kprintfln("%X %i %s\t%s", e->base, length, unit, type);

	}

	/* Load information into the kernel */
	pm_set_hhdm((void*) hhdm->offset);
	pm_allocate_table(memmap->entry_count);

	for (size_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry* e = memmap->entries[i];
		pm_add_map((void*) e->base, (size_t) e->length, (enum physmap_type) e->type);
	}

	total_physical_memory = available_physical_memory + kernel_size;
	kprintfln("physmgr: %i bytes enumerated, %i usable", total_physical_memory, available_physical_memory);

	return;
}

static void limine_get_kernel_address(void)
{

	/* The kernel cannot continue booting without knowning it's own address */
	if (kaddr_request.response == NULL) panic("no kernel address request answered.");

	struct limine_kernel_address_response* kaddr = kaddr_request.response;

	kernel_physical_base = (void*) kaddr->physical_base;
	kernel_virtual_base  = (void*) kaddr->virtual_base;

	kprintf("kernel physical %X\nkernel virtual  %X\n", kernel_physical_base, kernel_virtual_base);

	if ((intptr_t) kernel_virtual_base != (intptr_t) 0xFFFFFFFF80000000)
		panic("kernel in unexpected virtual address.");

	return;
}

volatile uint8_t __attribute__ ((aligned(4*KiB))) kernel_stack[32*KiB];

__attribute__ ((naked)) noreturn void start_kernel(void)
{
	__asm__ volatile
	(
		"movq	%0, %%rsp\n"
		"call	start_kernel2\n"
		:
		: "a" (kernel_stack)
		:
	);
}

/*
 * start_kernel2
 *
 * This is the amd64 entrypoint.
 *
 * After the conclusion of platform specific initialization, it jumps to kernel_main().
 */
noreturn void start_kernel2(void)
{

	com0_initialize_port();

	kprintln(VT_BOLD "Dagger 1.0 - Kernel Startup" VT_END);
	kprintfln("kernel stack at %X", &kernel_stack);

	/* Use Limine to get system info */
	limine_scan_boot_memmaps();
	limine_get_kernel_address();

	/* Initialize x86 specific features */
	gdt_init();
	idt_init();

	/* Switch from limine's pagetables to ours */
	pagetable_bootstrap_tables();

	/* Call into the platform agnostic portion of the kernel */
	kernel_main();

	panic("kernel_main returned.");

}
