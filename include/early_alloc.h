#ifndef KERNEL_EARLY_ALLOC_H_
#define KERNEL_EARLY_ALLOC_H_

#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>

#define EARLY_ALLOC_BUDGET 2*KiB

/*
 * This is the early boot bump alocator that pools from the raw, unitialized physical pages.
 * Some heap functionality is needed even at this early bootstage.
 *
 * Granularity is 1 byte, freeing is not possible.
 */

extern uint64_t early_alloc_memory_pool;
extern ssize_t  early_alloc_remaining;

void  early_alloc_set_memory_pool(void* pool);
void* early_alloc(size_t size);

ssize_t early_alloc_get_remaining(void);

#endif // KERNEL_EARLY_ALLOC_H_

