#ifndef KERNEL_PHYS_H_
#define KERNEL_PHYS_H_

#include <stdint.h>
#include <stddef.h>

/*
 * The type of an pmm.
 *
 * Mostly mimics Limine's way of catergorizing things.
 */
enum physical_memory_map_type
{
	pmmt_Usable                 = 0,    /* Usable   chunk of memory */
	pmmt_Reserved               = 1,    /* Unusable chunk of memory */
	pmmt_ACPI_Reclaimable       = 2,    /* Reclaimable memory, in current use by ACPI */
	pmmt_ACPI_NVS               = 3,    /* ACPI reserved memory */
	pmmt_Bad_Memory             = 4,    /* Unusable chunk of memory, scary edition */
	pmmt_Bootloader_Reclaimable = 5,    /* Reclaimable memory, in current use by the bootloader */
	pmmt_Kernel                 = 6,    /* The kernel. This catergory exists due to bootloader shenanigans */
	pmmt_Framebuffer            = 7     /* Graphical framebuffer given by bootloader */
};

/*
 * This struct represents a chunk of physical memory.
 *
 * - base:   Base address of map
 * - length: How large the chunk is (in bytes)
 * - type:   It's type
 */
struct physical_memory_map
{
	void*                    base;
	size_t                   length;
	enum physical_memory_map_type type;
};

/*
 * This singleton struct represents the physical memory of the system
 */
extern struct physical_memory_descriptor_table
{
	size_t capacity;
	size_t map_count;
	struct physical_memory_map* maps;
} physical_memory_descriptor;

extern void* early_boot_hhdm;
void pm_set_hhdm(void* hhdm);

void pm_allocate_table(unsigned int count);
void pm_add_map       (void* base, size_t length, enum physical_memory_map_type type);

#endif // KERNEL_PHYS_H

