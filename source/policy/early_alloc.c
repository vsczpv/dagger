#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>
#include <early_alloc.h>
#include <panic.h>

uint8_t early_alloc_memory_pool[EARLY_ALLOC_BUDGET] = {0};
size_t  early_alloc_used                            = 0;

void* early_alloc(size_t size)
{

	void* allc = &early_alloc_memory_pool[early_alloc_used];

	early_alloc_used      += size;

	if (early_alloc_used > EARLY_ALLOC_BUDGET) panic("kernel out of memory (early boot bump allocator)");

	return allc;
}

size_t early_alloc_get_remaining(void)
{
	return EARLY_ALLOC_BUDGET - early_alloc_used;
}
