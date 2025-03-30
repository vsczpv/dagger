/*
 * Copyright © 2025 Vinícius Schütz Piva
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

#ifndef KERNEL_SLAB_H_
#define KERNEL_SLAB_H_

#include <ext_stdint.h>

#define SLAB_MAGIC       0x21AB21AB21AB21AB
#define SLAB_MIN_STRIDE  8
#define SLAB_MAX_STRIDE  512

#define SLAB_COUNT       7

struct slab_cache;

/* A slab header that sits at the start of a slab buffer. */
struct slab_header
{
	uint64_t magic;           /* SLAB_MAGIC. */
	struct slab_header* more; /* The next page's worth of slabs. */
	struct slab_cache*  list; /* The list this slab belongs to. */
	size_t consumed;          /* How much of this cache has been consumed. */
	void*  slabhead;          /* The first free slab. */
};

struct slab_cache
{
	struct slab_header* head; /* The head of this slab cache list. */
	size_t stride;            /* The stride of this specific slab cache. */
	size_t empty;             /* The count of empty slab caches in this list. */
};

extern struct slab_cache slab_scs[SLAB_COUNT];

struct slab_header* slab_prepare(struct slab_cache* parent, void* mem);
int   slab_expand   (struct slab_cache* sc);
void  slab_kill     (struct slab_cache* sc, struct slab_header* sh);
void  slab_destroy  (struct slab_cache* sc);

int   slab_init     (struct slab_cache* sc, size_t stride);

void* slab_alloc(struct slab_cache* sc);
/*
 * void* slab_alloc(struct slab_cache* sc);
 *
 * Allocates a chunk of memory that comes from a slab in `sc`.
 *
 * May return ENOMEM.
 */

void  slab_free (void* what);
/*
 * void slab_free (void* what);
 *
 * Frees a chunk of memory allocated from any slab.
 */

void slab_subsystem_init(void);
void slab_subsystem_dump_info(void);

#endif // KERNEL_SLAB_H_

