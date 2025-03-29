#include <errno.h>
#include <memory.h>
#include <panic.h>
#include <minmax.h>
#include <phys.h>
#include <kernel.h>
#include <ktext.h>

void* klarge_alloc(size_t bytes)
{

	size_t pages = PAGE_COUNT_FOR_BYTES(bytes);

	/* I fear blowing up the stack */
	ASSERT(pages <= 256);

	intptr_t frames[pages];

	size_t dealloc_i = SIZE_MAX;

	for (size_t i = 0; i < pages; i++)
	{
		frames[i] = pm_pop_frame();
		if (frames[i] == (intptr_t) NULL)
			goto push_frames;
		dealloc_i = i;
	}

	void* virt = vmalloc(bytes);

	if (virt == (void*) ENOMEM)
		goto push_frames;

	struct pgdesc pgdesc =
	{
		.cmode   = PGDESC_PAT_WB,
		.global  = PGDESC_CONTEXTUAL,
		.rw      = PGDESC_READWRITE,
		.uk      = PGDESC_KERNEL,
		.xd      = PGDESC_EX_DISABLE,
		.present = PGDESC_PRESENT
	};

	int err = pg_map_frames(pages, virt, frames, &pgdesc);

	if (err == ENOMEM)
		goto do_vfree;

	ASSERT(err == 0);

	struct klarge_alloc_header* klah = virt;

	klah->magic = KLARGE_ALLOC_MAGIC;
	klah->pages = pages;

	return (char*) virt + sizeof (struct klarge_alloc_header);

do_vfree:

	vfree(virt, bytes);

push_frames:

	if (dealloc_i != SIZE_MAX) for (int i = dealloc_i; i >= 0; i--)
		pm_push_frame(frames[i]);

	return (void*) ENOMEM;
}

void klarge_free(void* what)
{

	struct klarge_alloc_header* head = (void*)((intptr_t) what & (~0xfff));

	ASSERT(head->magic == KLARGE_ALLOC_MAGIC);

	intptr_t frames[head->pages];

	size_t pages = head->pages;

	ASSERT(pg_unmap_frames(head->pages, head, frames) == 0);

	vfree(what, pages * PAGE_SIZE);

	for (size_t i = 0; i < pages; i++)
		pm_push_frame(frames[i]);

	return;
}

void* kmalloc(size_t bytes)
{

	if (bytes == 0)
		return NULL;

	if (bytes > SLAB_MAX_STRIDE)
		return klarge_alloc(bytes);

	bytes = next_power_of_2(bytes);
	bytes = MAX(SLAB_MIN_STRIDE, bytes);

	size_t order = BITLOG2(bytes) - BITLOG2(SLAB_MIN_STRIDE);

	ASSERT(order < SLAB_COUNT);

	return slab_alloc(&slab_scs[order]);
}

void kfree(void* what)
{

	if (what == NULL)
		return;

	uint64_t magic = *(uint64_t*)((intptr_t) what & (~0xfff));

	switch (magic)
	{
		case SLAB_MAGIC:
			slab_free(what);
			break;
		case KLARGE_ALLOC_MAGIC:
			klarge_free(what);
			break;
		default:
			panic("unknown allocation schema.");
	}

	return;
}
