#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>
#include <early_alloc.h>
#include <panic.h>

uint64_t early_alloc_memory_pool = 0;
ssize_t  early_alloc_remaining   = EARLY_ALLOC_BUDGET;

void early_alloc_set_memory_pool(void* pool)
{

	early_alloc_memory_pool = (uint64_t) pool;

	return;
}

void* early_alloc(size_t size)
{

	uint64_t allc = early_alloc_memory_pool;

	early_alloc_memory_pool += size;
	early_alloc_remaining   -= size;

	if (early_alloc_remaining < 0) panic("kernel out of memory (early boot bump allocator)");

	return (void*) allc;
}

ssize_t early_alloc_get_remaining(void)
{
	return early_alloc_remaining;
}
