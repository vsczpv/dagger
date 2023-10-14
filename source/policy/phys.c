#include <phys.h>
#include <early_alloc.h>
#include <panic.h>

struct physmap_descriptor_table physmap_descriptor =
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

	physmap_descriptor.capacity = count;
	physmap_descriptor.maps     = early_alloc(sizeof (struct physmap) * count);

	return;
}

void pm_add_map(void* base, size_t length, enum physmap_type type)
{

	struct physmap* map = &physmap_descriptor.maps[physmap_descriptor.map_count];

	map->base   = base;
	map->length = length;
	map->type   = type;

	physmap_descriptor.map_count++;

	if (physmap_descriptor.map_count > physmap_descriptor.capacity) panic("overflow in physmap_descriptor");

	return;
}
