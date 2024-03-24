#ifndef KERNEL_EARLY_ALLOC_H_
#define KERNEL_EARLY_ALLOC_H_

#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <ext_stdint.h>

#define EARLY_ALLOC_BUDGET 2*KiB

/*
 * This is the early boot bump alocator that pools from a small, fixed size array.
 * Some heap functionality is needed even at this early bootstage.
 *
 * Granularity is 1 byte, freeing is not possible.
 */

extern uint8_t early_alloc_memory_pool[EARLY_ALLOC_BUDGET];
extern size_t  early_alloc_used;

void*  early_alloc(size_t size);
size_t early_alloc_get_remaining(void);

#endif // KERNEL_EARLY_ALLOC_H_

