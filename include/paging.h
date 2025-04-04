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

#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_

#include <stdbool.h>
#include <ext_stdint.h>

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

#define PGDESC_RDONLY       0
#define PGDESC_READWRITE    1

#define PGDESC_USER         1
#define PGDESC_KERNEL       0

#define PGDESC_CONTEXTUAL   0
#define PGDESC_GLOBAL       1

#define PGDESC_EX_ENABLE    0
#define PGDESC_EX_DISABLE   1

#define PGDESC_NOT_PRESENT  0
#define PGDESC_PRESENT      1

struct pgdesc {
	intptr_t phys;
	uint8_t  cmode;
//	uint8_t  pk;
//	bool     cached;
	bool     rw;
	bool     uk;
	bool     global;
	bool     xd;
	bool     present;
};

int pg_unmap_frames(size_t count, void* virt, intptr_t phys[]);
/**
* int pg_unmap_frames (size_t count, void* virt, void** phys); -- Unmap `count` virtual address pages and store respective physical pages on `phys`.
*
* Is effectively a wrapper to
*
*     return pg_map_frames(count, virt, phys, { .present = PGDESC_NOT_PRESENT } )
*
* If `phys` is NULL the physical addresses are not stored.
*
* Errors:
*  Same as pg_map_frames
*
*/

int pg_map_frames(size_t count, void* virt, intptr_t phys[], struct pgdesc* pgdesc);
/**
* int pg_map_frames(size_t count, void* virt, void* phys[]); -- Map `count` virtual address pages into `count` physical frames.
*
* This function is used to map a series of non-contiguous physical frames into a equally sized and
* contiguous virtual address range. It applies to the page tables immediatly.
*
* Note that if pgdesc->present == PGDESC_NOT_PRESENT, pg_map_frames will unmap the pages instead.
* The frames that back `virt` will be placed on `phys[.count]`. If `phys` is NULL the addresses are discarded.
*
* Errors:
*  EINVAL - Either `virt` or `phys` are NULL or `count` is zero.
*  EFAULT - Either `virt` or and/some element(s) of `phys` ain't aligned to PAGE_SIZE.
*  ENOMEM - System has not enough memory to perform the mapping operation.
*  EDOM   - `virt` is not a canonical address.
*
* Example:
* {
*     // Preferably one should check if pop_frame() actually returned memory.
*     void* phys[4];
*     phys[0] = pop_frame();
*     phys[1] = pop_frame();
*     phys[2] = pop_frame();
*     phys[3] = pop_frame();
*     int err = pg_map_frames(4, 0xcafe0000, phys);
*     // Assuming pg_map_frames() succeded, `0xcafe3fff` is now a valid address
*     // pointing to four contiguous pages of virtual memory, backed by four frames of physical memory.
*     ((int*) phys[0]) = 1234;
* }
*/

#endif // KERNEL_PAGING_H_
