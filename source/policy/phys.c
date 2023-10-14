#include <phys.h>
#include <early_alloc.h>
#include <panic.h>

struct physical_memory_descriptor_table physical_memory_descriptor =
{
	.capacity  = 0,
	.map_count = 0,
	.maps      = NULL
};

void* early_boot_hhdm;

void pm_set_hhdm(void* hhdm)
{

	early_boot_hhdm = hhdm;

	return;
}

void pm_allocate_table(unsigned int count)
{

	physical_memory_descriptor.capacity = count;
	physical_memory_descriptor.maps     = early_alloc(sizeof (struct physical_memory_map) * count);

	return;
}

void pm_add_map(void* base, size_t length, enum physical_memory_map_type type)
{

	struct physical_memory_map* map = &physical_memory_descriptor.maps[physical_memory_descriptor.map_count];

	map->base   = base;
	map->length = length;
	map->type   = type;

	physical_memory_descriptor.map_count++;

	if (physical_memory_descriptor.map_count > physical_memory_descriptor.capacity) panic("overflow in physical_memory_descriptor");

	return;
}
