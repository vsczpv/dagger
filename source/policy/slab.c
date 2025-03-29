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

#include <slab.h>
#include <kernel.h>
#include <panic.h>
#include <ktext.h>
#include <phys.h>
#include <paging.h>
#include <buddy.h>
#include <errno.h>
#include <string.h>

struct slab_header* slab_prepare(struct slab_cache* parent, void* mem)
{

	ASSERT_PAGEALIGNED(mem);

	memset(mem, 0xca, 4096);

	struct slab_header* hdr = mem;

	hdr->magic    = SLAB_MAGIC;
	hdr->more     = NULL;
	hdr->list     = parent;
	hdr->consumed = 0;
	hdr->slabhead = (char*) mem + sizeof (struct slab_header);

	size_t max = (PAGE_SIZE - sizeof (struct slab_header)) / parent->stride;

	void** next_entry,** next_next;

	for (size_t i = 0; i < (max-1); i++)
	{
		size_t j = i + 1;

		next_entry = (void**) ((char*) mem + sizeof (struct slab_header) + sizeof(uint8_t)*parent->stride*i);
		next_next  = (void**) ((char*) mem + sizeof (struct slab_header) + sizeof(uint8_t)*parent->stride*j);

		*next_entry = next_next;
	}

	 next_entry = *next_entry;
	*next_entry = NULL;

	return (struct slab_header*) mem;
}

int slab_expand(struct slab_cache* sc)
{

	struct slab_header* head = sc->head;
	struct slab_header* tail = head;

	while (tail != NULL)
	{
		head = tail;
		tail = tail->more;
	}

	ASSERT(head != NULL);

	void* virt = vmalloc(PAGE_SIZE);

	if ((intptr_t) virt == ENOMEM)
		goto error;

	intptr_t frame = pm_pop_frame();

	if (frame == (intptr_t) NULL)
		goto unwind_virt;

	struct pgdesc pgdesc =
	{
		.cmode   = PGDESC_PAT_WB,
		.global  = PGDESC_CONTEXTUAL,
		.rw      = PGDESC_READWRITE,
		.uk      = PGDESC_KERNEL,
		.xd      = PGDESC_EX_DISABLE,
		.present = PGDESC_PRESENT
	};

	int err = pg_map_frames(1, virt, &frame, &pgdesc);

	if (err == ENOMEM)
		goto unwind_frame;

	ASSERT(err == 0);

	head->more = slab_prepare(sc, virt);

	sc->empty++;

	return 0;

unwind_virt:
	vfree(virt, PAGE_SIZE);
unwind_frame:
	ASSERT(pm_push_frame(frame) == 0);
error:
	return ENOMEM;
}

int slab_init(struct slab_cache* sc, size_t stride)
{

	void* virt = vmalloc(PAGE_SIZE);

	if ((intptr_t) virt == ENOMEM)
		goto error;

	intptr_t frame = pm_pop_frame();

	if (frame == (intptr_t) NULL)
		goto unwind_virt;

	struct pgdesc pgdesc =
	{
		.cmode   = PGDESC_PAT_WB,
		.global  = PGDESC_CONTEXTUAL,
		.rw      = PGDESC_READWRITE,
		.uk      = PGDESC_KERNEL,
		.xd      = PGDESC_EX_DISABLE,
		.present = PGDESC_PRESENT
	};

	int err = pg_map_frames(1, virt, &frame, &pgdesc);

	if (err == ENOMEM)
		goto unwind_frame;

	ASSERT(err == 0);

	sc->stride = stride;
	sc->empty  = 1;

	sc->head   = slab_prepare(sc, virt);

	return slab_expand(sc);

unwind_virt:
	vfree(virt, PAGE_SIZE);
unwind_frame:
	ASSERT(pm_push_frame(frame) == 0);
error:
	return ENOMEM;
}

void slab_kill(struct slab_cache* sc, struct slab_header* sh)
{

	struct slab_header* head = sc->head;
	struct slab_header* tail = head;

	while (tail != sh)
	{
		ASSERT(tail != NULL);
		head = tail;
		tail = tail->more;
	}

	/* Cutting slab out */
	head->more = tail->more;

	if (sh->consumed == 0)
		sc->empty--;

	intptr_t phys;
	ASSERT(pg_unmap_frames(1, sh, &phys) == 0);
	vfree(sh, PAGE_SIZE);
	pm_push_frame(phys);

	return;
}

void slab_destroy(struct slab_cache* sc)
{

	struct slab_header* tail = sc->head;

	while (tail != NULL)
	{

		struct slab_header* next = tail->more;

		intptr_t phys;
		ASSERT(pg_unmap_frames(1, tail, &phys) == 0);
		vfree(tail, PAGE_SIZE);
		pm_push_frame(phys);

		tail = next;
	}

	return;
}

void* slab_alloc(struct slab_cache* sc)
{

	struct slab_header* head = sc->head;

	size_t fit = (PAGE_SIZE - sizeof (struct slab_header)) / sc->stride;

	while (head != NULL)
	{
		ASSERT(head->consumed <= fit);
		if (head->consumed == fit)
		{
			head = head->more;
			continue;
		}

		if (head->consumed == 0)
			sc->empty--;

		head->consumed++;// += sc->stride;

		ASSERT(head->slabhead != NULL);

		void* res      = head->slabhead;
		head->slabhead = *((void**)head->slabhead);

		ASSERT(res != NULL);

		return res;
	}

	long int experr = slab_expand(sc);

	if (experr)
		return (void*) experr;

	return slab_alloc(sc);
}

void slab_free(void* what)
{

	struct slab_header* head = (void*)((intptr_t) what & (~0xfff));

	ASSERT(head->magic == SLAB_MAGIC);

	*((void**) what) = head->slabhead;
	head->slabhead   = what;
	head->consumed--;

	if (head->consumed == 0)
		head->list->empty++;

	if (head->list->empty >= 3)
	{
		ASSERT(head->list->empty == 3);
		slab_kill(head->list, head);
	}

	return;
}

struct slab_cache slab_scs[SLAB_COUNT];

void slab_subsystem_init(void)
{

	size_t stride = SLAB_MIN_STRIDE;

	for (int i = 0; i < SLAB_COUNT; i++)
	{
		ASSERT(stride <= SLAB_MAX_STRIDE);

		int err = slab_init(&slab_scs[i], stride);

		ASSERT(err == 0);

		stride = stride << 1;
	}

	kprintfln("slab: ready with %i caches.", SLAB_COUNT);

	return;
}

void slab_subsystem_dump_info(void)
{

	for (int i = 0; i < SLAB_COUNT; i++)
	{
		kprintfln("slab: 1 x %i: empty %i, with", slab_scs[i].stride, slab_scs[i].empty);
		struct slab_header* head = slab_scs[i].head;
		int j = 0;
		while (head != NULL)
		{
			kprintfln("\tbucket[%i]\t: %i\tconsumed\t%i bytes", j++, head->consumed, head->consumed * slab_scs[i].stride);

			head = head->more;
		}
	}

}
