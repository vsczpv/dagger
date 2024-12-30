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

#include <arch/amd64/pagetables.h>
#include <arch/amd64/cr.h>
#include <stddef.h>
#include <ext_stdint.h>
#include <ktext.h>
#include <kernel.h>
#include <panic.h>
#include <phys.h>
#include <minmax.h>
#include <memory.h>
#include <paging.h>

volatile struct pagetable_pml4_entry pagetables_root                  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pdpt_entry pagetables_kernelmap_pdpt_entries[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pd_entry   pagetables_kernelmap_pd_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pt_entry   pagetables_kernelmap_pt_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };

volatile struct pagetable_pdpt_entry pagetables_pfstack_pdpt_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pd_entry   pagetables_pfstack_pd_entries    [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pt_entry   pagetables_pfstack_pt_entries    [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };

volatile struct pagetable_pdpt_entry     pagetables_hhdm_pdpt_entries[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };
volatile struct pagetable_pd_entry_large pagetables_hhdm_pdl_entries [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096))) = { 0 };

#define PHYS(addr) (((intptr_t) addr) - (intptr_t) kernel_virtual_base + (intptr_t) kernel_physical_base)

/*
 * Map the kernel's physical frames to virtual pages.
 * We must do this by hand since our page manipulation codes
 * source from the physpage stack, which is not available during
 * early boot.
 */
static void pagetable_bootstrap_kernelspace(void)
{

	/* Indices */
	size_t idx_pgroot_kernelbase = PAGETABLES_GET_PML4_IDX(kernel_virtual_base);
	size_t idx_pgpdpt_kernelbase = PAGETABLES_GET_PDPT_IDX(kernel_virtual_base);
	size_t idx_pgpd_kernelbase   = PAGETABLES_GET_PD_IDX  (kernel_virtual_base);
	size_t idx_pgpt_kernelbase   = PAGETABLES_GET_PT_IDX  (kernel_virtual_base);

	volatile struct pagetable_pml4_entry *roote = &pagetables_root                  [idx_pgroot_kernelbase];
	volatile struct pagetable_pdpt_entry *pdpte = &pagetables_kernelmap_pdpt_entries[idx_pgpdpt_kernelbase];
	volatile struct pagetable_pd_entry   *pde   = &pagetables_kernelmap_pd_entries  [idx_pgpd_kernelbase];
	volatile struct pagetable_pt_entry   *pte   = &pagetables_kernelmap_pt_entries  [idx_pgpt_kernelbase];

	ASSERT(idx_pgpt_kernelbase == 0);

	/*
	 * We must map the whole kernel; it is physically contiguous, so doing it is
	 * as simple as using a for loop.
	 *
	 * NOTE: If kernel_virtual_base != 0xffff'ffff'8000'0000, this can overflow.
	 */
	for (ssize_t i = kernel_size / (4*KiB); i >= 0; i--)
	{

		ASSERT(i < PAGETABLE_ENTRIES_PER_TABLE);

		pte = &pagetables_kernelmap_pt_entries[i];
		pte->user_kern       = PGDESC_KERNEL;

		/* This must be RW because this whole holds both .text and .data */
		/* TODO: Not do that. */
		pte->read_write      = PGDESC_READWRITE;

		pte->phys_address    = ((intptr_t) kernel_physical_base >> 12) + i;

		pte->pat             = PGDESC_PAT_PAT(PGDESC_PAT_WB);
		pte->cache_disable   = PGDESC_PAT_PCD(PGDESC_PAT_WB);
		pte->wt_wb           = PGDESC_PAT_PWT(PGDESC_PAT_WB);

		pte->execute_disable = PGDESC_EX_ENABLE;
		pte->global          = PGDESC_GLOBAL;

		pte->pke             = 0; /* undefined */
		pte->accessed        = 0;
		pte->dirty           = 0;
		pte->reserved        = 0;
		pte->present         = 1;
	}

	pde->reserved        = 0;
	pde->phys_address    = PHYS(pte) >> 12;
	pde->user_kern       = PGDESC_KERNEL;
	pde->read_write      = PGDESC_READWRITE;
	pde->execute_disable = PGDESC_EX_ENABLE;
	pde->cache_disable   = PGDESC_CACHED;
	pde->wt_wb           = PGDESC_WRITEBACK;
	pde->page_size       = PGDESC_PAGE_NORMAL;
	pde->accessed        = 0;
	pde->present         = 1;

	pdpte->reserved        = 0;
	pdpte->phys_address    = PHYS(pde) >> 12;
	pdpte->user_kern       = PGDESC_KERNEL;
	pdpte->read_write      = PGDESC_READWRITE;
	pdpte->execute_disable = PGDESC_EX_ENABLE;
	pdpte->cache_disable   = PGDESC_CACHED;
	pdpte->wt_wb           = PGDESC_WRITEBACK;
	pdpte->page_size       = PGDESC_PAGE_NORMAL;
	pdpte->accessed        = 0;
	pdpte->present         = 1;

	roote->reserved_1      = 0;
	roote->reserved_2      = 0;
	roote->phys_address    = PHYS(pdpte) >> 12;
	roote->user_kern       = PGDESC_KERNEL;
	roote->read_write      = PGDESC_READWRITE;
	roote->execute_disable = PGDESC_EX_ENABLE;
	roote->cache_disable   = PGDESC_CACHED;
	roote->wt_wb           = PGDESC_WRITEBACK;
	roote->accessed        = 0;
	roote->present         = 1;

	return;
}

/*
 * Maps the physmgr's stack into some physical memory. Note that currently we
 * are capped at one terabyte worth of managed memory (out of 64).
 */
static void pagetable_bootstrap_physmgr(void)
{

	/* TODO: check math */
	size_t used_pages   = 0;
	size_t needed_pages = (available_physical_memory / (4*KiB) * 8) / (4*KiB) + 1;

	struct physmap* map = pm_find_physmap_with_atleast(needed_pages*4*KiB);


	/*
	 * By definition, the memory used for allocator bookeeping is
	 * smaller than the full available memory range, as a 1:1 or even
	 * larger ratio of memory-to-bookeep would be quite inefficient.
	 */
	ASSERT(map != NULL);

	/* Page aligned */
	ASSERT(((intptr_t) map->base & 0b1111'1111'1111) == 0);

	/*
	 * Technically we should be able to allocate the full 64 TiB our
	 * layout permits us, but limiting ourselves to 1 TiB makes the process
	 * of initializing the physmgr simpler.
	 * TODO: Not do that.
	 */

	intptr_t* pf_stack = PHYSFRAME_STACK_GETPTR();

	size_t idx_pgroot_pfstack = PAGETABLES_GET_PML4_IDX(pf_stack);
	size_t idx_pgpdpt_pfstack = PAGETABLES_GET_PDPT_IDX(pf_stack);
	size_t idx_pgpd_pfstack   = PAGETABLES_GET_PD_IDX  (pf_stack);
	size_t idx_pgpt_pfstack   = PAGETABLES_GET_PT_IDX  (pf_stack);

	ASSERT(idx_pgpt_pfstack == 0);

	volatile struct pagetable_pml4_entry *roote = &pagetables_root                [idx_pgroot_pfstack];
	volatile struct pagetable_pdpt_entry *pdpte = &pagetables_pfstack_pdpt_entries[idx_pgpdpt_pfstack];
	volatile struct pagetable_pd_entry   *pde   = &pagetables_pfstack_pd_entries  [idx_pgpd_pfstack];
	volatile struct pagetable_pt_entry   *pte;//   = &pagetables_pfstack_pt_entries  [idx_pgpt_pfstack];

	for (ssize_t i = needed_pages - 1; i >= 0; i--)
	{

		ASSERT(i < PAGETABLE_ENTRIES_PER_TABLE);

		pte = &pagetables_pfstack_pt_entries[i];
		pte->user_kern       = PGDESC_KERNEL;
		pte->read_write      = PGDESC_READWRITE;
		pte->phys_address    = ((intptr_t) map->base >> 12) + i;

		pte->pat             = PGDESC_PAT_PAT(PGDESC_PAT_WB);
		pte->cache_disable   = PGDESC_PAT_PCD(PGDESC_PAT_WB);
		pte->wt_wb           = PGDESC_PAT_PWT(PGDESC_PAT_WB);

		pte->execute_disable = PGDESC_EX_DISABLE;
		pte->global          = PGDESC_GLOBAL;

		pte->pke             = 0; /* undefined */
		pte->accessed        = 0;
		pte->dirty           = 0;
		pte->reserved        = 0;
		pte->present         = 1;

		used_pages++;
	}

	size_t old = map->length;

	map->base    = (void*) ((intptr_t) map->base + (used_pages << 12));
	map->length -= used_pages << 12;

	/* Just to be sure... */
	ASSERT(old > map->length);

	if (map->length == 0) map->reaped = true;

	pde->reserved        = 0;
	pde->phys_address    = PHYS(pte) >> 12;
	pde->user_kern       = PGDESC_KERNEL;
	pde->read_write      = PGDESC_READWRITE;
	pde->execute_disable = PGDESC_EX_DISABLE;
	pde->cache_disable   = PGDESC_CACHED;
	pde->wt_wb           = PGDESC_WRITEBACK;
	pde->page_size       = PGDESC_PAGE_NORMAL;
	pde->accessed        = 0;
	pde->present         = 1;

	pdpte->reserved        = 0;
	pdpte->phys_address    = PHYS(pde) >> 12;
	pdpte->user_kern       = PGDESC_KERNEL;
	pdpte->read_write      = PGDESC_READWRITE;
	pdpte->execute_disable = PGDESC_EX_DISABLE;
	pdpte->cache_disable   = PGDESC_CACHED;
	pdpte->wt_wb           = PGDESC_WRITEBACK;
	pdpte->page_size       = PGDESC_PAGE_NORMAL;
	pdpte->accessed        = 0;
	pdpte->present         = 1;

	roote->reserved_1      = 0;
	roote->reserved_2      = 0;
	roote->phys_address    = PHYS(pdpte) >> 12;
	roote->user_kern       = PGDESC_KERNEL;
	roote->read_write      = PGDESC_READWRITE;
	roote->execute_disable = PGDESC_EX_DISABLE;
	roote->cache_disable   = PGDESC_CACHED;
	roote->wt_wb           = PGDESC_WRITEBACK;
	roote->accessed        = 0;
	roote->present         = 1;

	available_physical_memory -= used_pages * 4*KiB;
	kprintfln("physmgr: %i bytes used for bookeeping.", used_pages * 4*KiB);

	return;
}

/*
 * Maps the first gigabyte of the hhdm. The first two megabytes are skipped.
 */
static void pagetable_bootstrap_lower_hhdm(void)
{

	struct physmap* map = pm_find_physmap_highest_usable();

	/*
	 * We don't trust our own code that much yet
	 * so let's limit the kernel to one GiB.
	 */

	intptr_t sz = (intptr_t) map->base + map->length;

	ASSERT((intptr_t) sz < 1*GiB);

	sz = MIN(sz, 1*GiB-1);

	kprintfln("physmgr: mapping %i bytes worth of physical bus.", sz);

	size_t needed_pages = (sz / (2*MiB)) + 1;

	void* lower_hhdm = HHDM_GETPTR();

	size_t idx_pgroot_lowerhhdm = PAGETABLES_GET_PML4_IDX(lower_hhdm);
	size_t idx_pgpdpt_lowerhhdm = PAGETABLES_GET_PDPT_IDX(lower_hhdm);
	size_t idx_pgpdl_lowerhhdm  = PAGETABLES_GET_PD_IDX  (lower_hhdm);

	volatile struct pagetable_pml4_entry     *roote = &pagetables_root             [idx_pgroot_lowerhhdm];
	volatile struct pagetable_pdpt_entry     *pdpte = &pagetables_hhdm_pdpt_entries[idx_pgpdpt_lowerhhdm];

	ASSERT(idx_pgpdl_lowerhhdm == 0);

	intptr_t phys = 0x0;

	for (size_t i = 0; i < needed_pages; i++)
	{

		ASSERT(i < PAGETABLE_ENTRIES_PER_TABLE);

		volatile struct pagetable_pd_entry_large *pdle = &pagetables_hhdm_pdl_entries[i];

		pdle->page_size       = PGDESC_PAGE_BIG;

		pdle->user_kern       = PGDESC_KERNEL;
		pdle->read_write      = PGDESC_READWRITE;

		pdle->phys_address    = (phys & PGDESC_PHYSADDR_LARGE_MASK) >> PGDESC_PHYSADDR_LARGE_SHIFT;

		pdle->pat             = PGDESC_PAT_PAT(PGDESC_PAT_WB);
		pdle->cache_disable   = PGDESC_PAT_PCD(PGDESC_PAT_WB);
		pdle->wt_wb           = PGDESC_PAT_PWT(PGDESC_PAT_WB);

		pdle->execute_disable = PGDESC_EX_DISABLE;
		pdle->global          = PGDESC_GLOBAL;

		pdle->pke             = 0; /* undefined */
		pdle->accessed        = 0;
		pdle->dirty           = 0;
		pdle->reserved        = 0;
		pdle->present         = 1;

		phys += 2*MiB;
	}

	pdpte->reserved        = 0;
	pdpte->phys_address    = PHYS(&pagetables_hhdm_pdl_entries[0]) >> 12;
	pdpte->user_kern       = PGDESC_KERNEL;
	pdpte->read_write      = PGDESC_READWRITE;
	pdpte->execute_disable = PGDESC_EX_DISABLE;
	pdpte->cache_disable   = PGDESC_CACHED;
	pdpte->wt_wb           = PGDESC_WRITEBACK;
	pdpte->page_size       = PGDESC_PAGE_NORMAL;
	pdpte->accessed        = 0;
	pdpte->present         = 1;

	roote->reserved_1      = 0;
	roote->reserved_2      = 0;
	roote->phys_address    = PHYS(pdpte) >> 12;
	roote->user_kern       = PGDESC_KERNEL;
	roote->read_write      = PGDESC_READWRITE;
	roote->execute_disable = PGDESC_EX_DISABLE;
	roote->cache_disable   = PGDESC_CACHED;
	roote->wt_wb           = PGDESC_WRITEBACK;
	roote->accessed        = 0;
	roote->present         = 1;

	return;
}

void pagetable_reload_pages(volatile struct pagetable_pml4_entry root[PAGETABLE_ENTRIES_PER_TABLE])
{

	intptr_t cr3 = __read_cr3();

	membarrier();

	ASSERT((((intptr_t) root) & 0xfff) == 0)

	cr3 &= 0b1111'1111'1111;
	cr3 |= ((intptr_t) PHYS(root)) & 0xffff'ffff'ffff'f000;

	membarrier();

	__write_cr3(cr3);

	membarrier();
}

void pagetable_bootstrap_tables(void)
{

	kprintfln("limine:  pagetable root at %X", (intptr_t) __read_cr3());
	kprintfln("kernel's pagetable root at %X", (intptr_t) PHYS(&pagetables_root));

	pagetable_bootstrap_kernelspace();
	pagetable_bootstrap_physmgr();
	pagetable_bootstrap_lower_hhdm();

	pagetable_reload_pages(pagetables_root);

	kprintfln("paging: switching to our pagetables (%X)", __read_cr3());

	return;
}
