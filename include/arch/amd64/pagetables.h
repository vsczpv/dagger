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

#ifndef KERNEL_ARCH_AM64_PAGETABLES_H_
#define KERNEL_ARCH_AM64_PAGETABLES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct __attribute__ ((packed)) pagetable_pt_entry
{
	uint64_t present         : 1;
	uint64_t read_write      : 1;
	uint64_t user_kern       : 1;
	uint64_t wt_wb           : 1;
	uint64_t cache_disable   : 1;
	uint64_t accessed        : 1;
	uint64_t dirty           : 1;
	uint64_t pat             : 1;
	uint64_t global          : 1;
	uint64_t available_lo    : 3;
	uint64_t phys_address    : 36;
	uint64_t reserved        : 4;
	uint64_t available_hi    : 7;
	uint64_t pke             : 4;
	uint64_t execute_disable : 1;
};

struct __attribute__ ((packed)) pagetable_pd_entry
{
	uint64_t present         : 1;
	uint64_t read_write      : 1;
	uint64_t user_kern       : 1;
	uint64_t wt_wb           : 1;
	uint64_t cache_disable   : 1;
	uint64_t accessed        : 1;
	uint64_t available_lo    : 1;
	uint64_t page_size       : 1;	/* NOTE: MUST BE PGDESC_PAGE_NORMAL! */
	uint64_t available_mi    : 4;
	uint64_t phys_address    : 36;
	uint64_t reserved        : 4;
	uint64_t available_hi    : 11;
	uint64_t execute_disable : 1;
};

#define PGDESC_PHYSADDR_LARGE_MASK  0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'0000'0000'0000'0000'0000'0000
#define PGDESC_PHYSADDR_LARGE_SHIFT 13

/* 2 MiB Variant */
struct __attribute__ ((packed)) pagetable_pd_entry_large
{
	uint64_t present         : 1;
	uint64_t read_write      : 1;
	uint64_t user_kern       : 1;
	uint64_t wt_wb           : 1;
	uint64_t cache_disable   : 1;
	uint64_t accessed        : 1;
	uint64_t dirty           : 1;
	uint64_t page_size       : 1;  /* NOTE: MUST BE PGDESC_PAGE_BIG! */
	uint64_t global          : 1;
	uint64_t available_lo    : 3;  /* NOTE: Apparently the osdev.wiki and the intel manual disagree if
	                                        the last bit of this is reserved or not. */
	uint64_t pat             : 1;
	uint64_t phys_address    : 35; /* NOTE: Lower 12 bits must be zero (reserved). */
	uint64_t reserved        : 4;
	uint64_t available_hi    : 7;
	uint64_t pke             : 4;
	uint64_t execute_disable : 1;
};

/* Identical */
#define pagetable_pdpt_entry pagetable_pd_entry

struct __attribute__ ((packed)) pagetable_pml4_entry
{
	uint64_t present         : 1;
	uint64_t read_write      : 1;
	uint64_t user_kern       : 1;
	uint64_t wt_wb           : 1;
	uint64_t cache_disable   : 1;
	uint64_t accessed        : 1;
	uint64_t available_lo    : 1;
	uint64_t reserved_1      : 1;
	uint64_t available_mi    : 4;
	uint64_t phys_address    : 36;
	uint64_t reserved_2      : 4;
	uint64_t available_hi    : 11;
	uint64_t execute_disable : 1;
};

/* 512 = 4*KiB / sizeof(struct pagetable_pml4_entry) */
#define PAGETABLE_ENTRIES_PER_TABLE 512
extern volatile struct pagetable_pml4_entry pagetables_root[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));

/*
 * These hold the initial mappings that pin the kernel in place
 * Further mapping are sourced from the physpage stack and are put
 * in the higher half direct map of physical memory.
 *
 * Since they include themselves, there is no bootstrap problem to solve ;)
 *
 * This limits the kernel to two megabytes in size.
 */
extern volatile struct pagetable_pdpt_entry pagetables_kernelmap_pdpt_entries[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));
extern volatile struct pagetable_pd_entry   pagetables_kernelmap_pd_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));
extern volatile struct pagetable_pt_entry   pagetables_kernelmap_pt_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));

/*
 * These are for the physframe stack.
 */
extern volatile struct pagetable_pdpt_entry pagetables_pfstack_pdpt_entries[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));
extern volatile struct pagetable_pd_entry   pagetables_pfstack_pd_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));
extern volatile struct pagetable_pt_entry   pagetables_pfstack_pt_entries  [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));

/*
 * These are for the hhdm.
 */
extern volatile struct pagetable_pdpt_entry     pagetables_hhdm_pdpt_entries[PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));
extern volatile struct pagetable_pd_entry_large pagetables_hhdm_pdl_entries [PAGETABLE_ENTRIES_PER_TABLE] __attribute__ ((aligned(4096)));

void pagetable_bootstrap_tables(void);

/* CCCC'CCCC CCCC'CCCC | XXXX'XXXX X000'0000 : 0000'0000 0000'0000 | 0000'0000 0000'0000 */
#define PAGETABLES_IDX_PML4_MASK  0xff80'0000'0000
#define PAGETABLES_IDX_PML4_SHIFT 39
#define PAGETABLES_GET_PML4_IDX(addr) (((intptr_t) (addr) & PAGETABLES_IDX_PML4_MASK) >> PAGETABLES_IDX_PML4_SHIFT)

/* CCCC'CCCC CCCC'CCCC | ----'---- -XXX'XXXX : XX00'0000 0000'0000 | 0000'0000 0000'0000 */
#define PAGETABLES_IDX_PDPT_MASK  0x7f'c000'0000
#define PAGETABLES_IDX_PDPT_SHIFT 30
#define PAGETABLES_GET_PDPT_IDX(addr) (((intptr_t) (addr) & PAGETABLES_IDX_PDPT_MASK) >> PAGETABLES_IDX_PDPT_SHIFT)

/* CCCC'CCCC CCCC'CCCC | ----'---- ----'---- : --XX'XXXX XXX0'0000 | 0000'0000 0000'0000 */
#define PAGETABLES_IDX_PD_MASK    0x3fe0'0000
#define PAGETABLES_IDX_PD_SHIFT   21
#define PAGETABLES_GET_PD_IDX(addr) (((intptr_t) (addr) & PAGETABLES_IDX_PD_MASK) >> PAGETABLES_IDX_PD_SHIFT)

/* CCCC'CCCC CCCC'CCCC | ----'---- ----'---- : ----'---- ---X'XXXX | XXXX'0000 0000'0000 */
#define PAGETABLES_IDX_PT_MASK    0x1f'f000
#define PAGETABLES_IDX_PT_SHIFT   12
#define PAGETABLES_GET_PT_IDX(addr) (((intptr_t) (addr) & PAGETABLES_IDX_PT_MASK) >> PAGETABLES_IDX_PT_SHIFT)


#define PGDESC_WRITEBACK    0
#define PGDESC_WRITETHROUGH 1
#define PGDESC_CACHED       0
#define PGDESC_UNCACHED     1

#define PGDESC_PAGE_NORMAL  0
#define PGDESC_PAGE_BIG     1

#define PGDESC_PAT_WB       0
#define PGDESC_PAT_WT       1
#define PGDESC_PAT_UC_MINUS 2
#define PGDESC_PAT_UC       3
#define PGDESC_PAT_WP       4
#define PGDESC_PAT_WC       5

#define PGDESC_PAT_PAT(mode) ((mode & 0b100) >> 2)
#define PGDESC_PAT_PCD(mode) ((mode & 0b010) >> 1)
#define PGDESC_PAT_PWT(mode) ((mode & 0b001))

#define PGDESC_RDONLY       0
#define PGDESC_READWRITE    1

#define PGDESC_USER         0
#define PGDESC_KERNEL       1

#define PGDESC_CONTEXTUAL   0
#define PGDESC_GLOBAL       1

#define PGDESC_EX_ENABLE    0
#define PGDESC_EX_DISABLE   1

struct pgdesc {
	intptr_t phys;
	uint8_t  pat;
//	uint8_t  pk;
	bool     rw;
	bool     uk;
	bool     global;
	bool     xd;
};

void pagetable_reload_pages(volatile struct pagetable_pml4_entry root[PAGETABLE_ENTRIES_PER_TABLE]);

void __pml4_map_frames(struct pagetable_pml4_entry pml4[PAGETABLE_ENTRIES_PER_TABLE], void* virt_begin, size_t length, intptr_t phys[], struct pgdesc* desc);

/*
__pml4_map_frames (struct pml4e pml4[], void* virt_begin, void* virt_end, void* phys[], struct pgdesc* desc)
__pdpt_map_frames (struct pdpte pdpt[], void* virt_begin, void* virt_end, void* phys[], struct pgdesc* desc, size_t pidx)
__pd_map_frames   (struct pde   pd  [], void* virt_begin, void* virt_end, void* phys[], struct pgdesc* desc, size_t pidx)
__pt_map_frames   (struct pte   pt  [], void* virt_begin, void* virt_end, void* phys[], struct pgdesc* desc, size_t pidx)

__pml4_unmap_frames (struct pml4e pml4[], void* virt_begin, void* virt_end, void* phys[])
__pdpt_unmap_frames (struct pdpte pdpt[], void* virt_begin, void* virt_end, void* phys[], size_t pidx)
__pd_unmap_frames   (struct pde   pd  [], void* virt_begin, void* virt_end, void* phys[], size_t pidx)
__pt_unmap_frames   (struct pte   pt  [], void* virt_begin, void* virt_end, void* phys[], size_t pidx)

// Contiguous
__pml4_map_frame_range (struct pml4e pml4[], void* virt_begin, void* virt_end, void* phys_begin, struct pgdesc* desc)
__pdpt_map_frame_range (struct pdpte pdpt[], void* virt_begin, void* virt_end, void* phys_begin, struct pgdesc* desc)
__pd_map_frame_range   (struct pde   pd  [], void* virt_begin, void* virt_end, void* phys_begin, struct pgdesc* desc)
__pt_map_frame_range   (struct pte   pt  [], void* virt_begin, void* virt_end, void* phys_begin, struct pgdesc* desc)
*/

// extern size_t architectural_physaddr_limit;

//struct pagetable_pte_entry pagetable_pte_entry_create(size_t physaddr, bool present, bool rw, bool prot, bool cache_mode, uint8_t pat_mode, bool global, bool xd);

//struct pagetable_pde_entry pagetable_pde_entry_create()

#endif // KERNEL_ARCH_AM64_PAGETABLES_H_
