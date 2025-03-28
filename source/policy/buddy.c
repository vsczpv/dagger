#include <buddy.h>
#include <panic.h>
#include <errno.h>

#include <ktext.h>

uint8_t vmalloc_area_buddy_list[VMALLOC_AREA_BUDDY_LIST_SIZE];

struct buddy_region vmalloc_buddy =
{
	.array    = vmalloc_area_buddy_list,
	.buddylen = VMALLOC_AREA_BUDDY_LIST_SIZE_BITS
};

inline static size_t next_power_of_2(size_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

ssize_t buddy_alloc(struct buddy_region* buddy, size_t ct)
{

	size_t len = next_power_of_2(ct);

	if (len >= buddy->buddylen/2)
		return -ENOMEM;

	for (size_t i = 0; i < buddy->buddylen; i += len) {

		if (BITOF(buddy->array, i) == 1) continue;

		ASSERT((i + len) <= buddy->buddylen);

		for (size_t j = i; j < (i + len); j++) {
			SETBIT(buddy->array, j, 1);
		}

		return i;
	}

	return -ENOMEM;
}

void buddy_free(struct buddy_region* buddy, ssize_t pos, size_t ct)
{

	ASSERT(pos >= 0);

	size_t len = next_power_of_2(ct);

	for (size_t i = (unsigned) pos; i < (pos + len); i++)  {
		SETBIT(buddy->array, i, 0);
	}

	return;
}

void* vmalloc(size_t bytes)
{
	size_t pages = bytes / 4 / KiB;

	ssize_t buddyid = buddy_alloc(&vmalloc_buddy, pages);

	if (buddyid < 0)
		return (void*) -buddyid;

	return (void*) (VMALLOC_AREA_LOCATION + buddyid * 4 * KiB);
}

void  vfree(void* vaddr, size_t bytes)
{

	size_t  pages   = bytes / 4 / KiB;
	ssize_t buddyid = ((intptr_t) vaddr - VMALLOC_AREA_LOCATION) / 4 / KiB;

	buddy_free(&vmalloc_buddy, buddyid, pages);

	return;
}
