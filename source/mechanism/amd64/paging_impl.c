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
#include <paging.h>
#include <errno.h>
#include <string.h>
#include <kernel.h>
#include <panic.h>
#include <arch/amd64/pagetables.h>

#include <ktext.h>

static size_t __pg_map_frames_pt(volatile struct pagetable_pt_entry* pt_root, size_t count, void* virt, intptr_t phys[], struct pgdesc* pgdesc, size_t total)
{

	kprintln("D");

	size_t mapped = 0;
	size_t idx_pt = PAGETABLES_GET_PT_IDX(virt);

	while (idx_pt < PAGETABLE_ENTRIES_PER_TABLE && count != 0)
	{

		kprintfln("E %i", idx_pt);

		volatile struct pagetable_pt_entry* pte = &pt_root[idx_pt];

		kprintfln("F %X", pte);

		ASSERT(pte->present == 0);

		kprintfln("G %x %x %x", total, count, count/4/KiB);

		size_t   physidx = total - count/4/KiB;

		kprintfln("H %X, %X", physidx, phys + 1);

		intptr_t frame   = phys[physidx];

		kprintfln("I %X\t:\t:%X", frame, pte);

		pte->phys_address    = frame >> 12;
		pte->user_kern       = pgdesc->uk;
		pte->read_write      = pgdesc->rw;
		pte->execute_disable = pgdesc->xd;
		pte->cache_disable   = PGDESC_PAT_PCD(pgdesc->cmode);
		pte->wt_wb           = PGDESC_PAT_PWT(pgdesc->cmode);
		pte->pat             = PGDESC_PAT_PAT(pgdesc->cmode);
		pte->global          = pgdesc->global;
		pte->accessed        = 0;
		pte->present         = 1;

		mapped += 4*KiB;
		count  -= 4*KiB;
		idx_pt++;
	}

	kprintln("Z");
	return mapped;
}

static size_t __pg_map_frames_pd(volatile struct pagetable_pd_entry* pd_root, size_t count, void* virt, intptr_t phys[], struct pgdesc* pgdesc, size_t total)
{

	kprintln("C");

	size_t mapped_tot = 0;
	size_t idx_pd     = PAGETABLES_GET_PD_IDX(virt);

	while (idx_pd < PAGETABLE_ENTRIES_PER_TABLE && count != 0)
	{
		volatile struct pagetable_pd_entry* pde = &pd_root[idx_pd];
		volatile struct pagetable_pt_entry* pt_root;

		if (pde->present == 0)
		{
			intptr_t tableaddr = pm_pop_frame();
			ASSERT  (tableaddr != (intptr_t) NULL);

			void* vaddr = HHDM_PHYS_TO_VIRT(tableaddr);
			(void) memset(vaddr, 0, sizeof (struct pagetable_pt_entry) * PAGETABLE_ENTRIES_PER_TABLE);
			pt_root     = (volatile struct pagetable_pt_entry*) vaddr;

			pde->phys_address    = tableaddr >> 12;
			pde->user_kern       = pgdesc->uk;
			pde->read_write      = pgdesc->rw;
			pde->execute_disable = pgdesc->xd;
			pde->cache_disable   = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			pde->wt_wb           = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			pde->accessed        = 0;
			pde->present         = 1;
		}

		else
		{
			intptr_t tableaddr = pde->phys_address << 12;
			void*    vaddr     = HHDM_PHYS_TO_VIRT(tableaddr);
			pt_root            = (volatile struct pagetable_pt_entry*) vaddr;
		}

		size_t mapped = __pg_map_frames_pt(pt_root, count, virt, phys, pgdesc, total);
		count        -= mapped;
		virt          = (void*) ((intptr_t) virt + mapped);

		mapped_tot += mapped;
		idx_pd++;
	}

	return mapped_tot;
}

static size_t __pg_map_frames_pdpt(volatile struct pagetable_pdpt_entry* dt_root, size_t count, void* virt, intptr_t phys[], struct pgdesc* pgdesc, size_t total)
{

	kprintln("B");

	size_t mapped_tot = 0;
	size_t idx_dt     = PAGETABLES_GET_PDPT_IDX(virt);

	while (idx_dt < PAGETABLE_ENTRIES_PER_TABLE && count != 0)
	{

		volatile struct pagetable_pdpt_entry* dte = &dt_root[idx_dt];
		volatile struct pagetable_pd_entry*   pd_root;

		if (dte->present == 0)
		{
			intptr_t tableaddr = pm_pop_frame();
			ASSERT  (tableaddr != (intptr_t) NULL);

			void*  vaddr = HHDM_PHYS_TO_VIRT(tableaddr);
			(void) memset(vaddr, 0, sizeof (struct pagetable_pd_entry) * PAGETABLE_ENTRIES_PER_TABLE);
			pd_root      = (volatile struct pagetable_pd_entry*) vaddr;

			dte->phys_address    = tableaddr >> 12;
			dte->user_kern       = pgdesc->uk;
			dte->read_write      = pgdesc->rw;
			dte->execute_disable = pgdesc->xd;
			dte->cache_disable   = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			dte->wt_wb           = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			dte->accessed        = 0;
			dte->present         = 1;
		}

		else
		{
			intptr_t tableaddr = dte->phys_address << 12;
			void*    vaddr     = HHDM_PHYS_TO_VIRT(tableaddr);
			pd_root            = (volatile struct pagetable_pd_entry*) vaddr;
		}

		size_t mapped = __pg_map_frames_pd(pd_root, count, virt, phys, pgdesc, total);
		count        -= mapped;
		virt          = (void*) ((intptr_t) virt + mapped);

		mapped_tot += mapped;
		idx_dt++;
	}

    return mapped_tot;
}

int pg_map_frames(size_t count, void* virt, intptr_t phys[], struct pgdesc* pgdesc)
{

	kprintln("A");

	count *= 4*KiB;

	size_t max_needed_pages  = count / (PAGETABLE_ENTRIES_PER_TABLE) + 1;
	       max_needed_pages += count / (PAGETABLE_ENTRIES_PER_TABLE*PAGETABLE_ENTRIES_PER_TABLE) + 1;
		   max_needed_pages += count / (PAGETABLE_ENTRIES_PER_TABLE*PAGETABLE_ENTRIES_PER_TABLE*PAGETABLE_ENTRIES_PER_TABLE) + 1;

	/*
	 * NOTE: In the future, locking the pm subsystem may be needed as this invariant
     *       must not be invalidaded throughout the duration of this function's execution.
	 */
    if ((available_physical_memory / 4*KiB) <= max_needed_pages)
		return ENOMEM;
		/* System does not have enough physical memory to guarantee mapping success
         * on worse case scenario. */

	if ((intptr_t) (((intptr_t) virt) + count) < (intptr_t) virt)
		return EDOM;
		/* Address range overflows. */

	if (!CANONICAL(virt) || count == 0 || phys == NULL)
		return EINVAL;
		/* Address range not canonical; inval arg. */

	size_t idx_ro = PAGETABLES_GET_PML4_IDX(virt);
	size_t total  = count/4/KiB;

	while (idx_ro < PAGETABLE_ENTRIES_PER_TABLE && count != 0)
	{

		volatile struct pagetable_pml4_entry* roe = &pagetables_root[idx_ro];
		volatile struct pagetable_pdpt_entry* dt_root;

		if (roe->present == 0)
		{
			intptr_t tableaddr = pm_pop_frame();
			ASSERT  (tableaddr != (intptr_t) NULL);

			void*  vaddr = HHDM_PHYS_TO_VIRT(tableaddr);
			(void) memset(vaddr, 0, sizeof (struct pagetable_pdpt_entry) * PAGETABLE_ENTRIES_PER_TABLE);
			dt_root      = (volatile struct pagetable_pdpt_entry*) vaddr;

			roe->phys_address    = tableaddr >> 12;
			roe->user_kern       = pgdesc->uk;
			roe->read_write      = pgdesc->rw;
			roe->execute_disable = pgdesc->xd;
			roe->cache_disable   = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			roe->wt_wb           = pgdesc->cmode == PGDESC_PAT_WB ? PGDESC_CACHED : PGDESC_UNCACHED; /* TODO: Revisit */
			roe->accessed        = 0;
			roe->present         = 1;
		}

		else
		{
			intptr_t tableaddr = roe->phys_address << 12;
			void*    vaddr     = HHDM_PHYS_TO_VIRT(tableaddr);
			dt_root            = (volatile struct pagetable_pdpt_entry*) vaddr;
		}

		size_t mapped = __pg_map_frames_pdpt(dt_root, count, virt, phys, pgdesc, total);
		count        -= mapped;
		virt          = (void*) ((intptr_t) virt + mapped);

		idx_ro++;
	}

	pagetable_reload_pages(pagetables_root);

	return 0;
}