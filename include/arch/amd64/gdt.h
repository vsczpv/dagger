#ifndef KERNEL_ARCH_AMD64_GDT_H_
#define KERNEL_ARCH_AMD64_GDT_H_

#include <stdint.h>
#include <stddef.h>

/*
 * This struct represents an entry in the globar descriptor table.
 * It's quite the finnicky thing, and thus must always be maninpulated using the API.
 *
 * Note that limit and base are ignored in amd64
 */
struct __attribute__ ((packed)) gdt_entry
{

	/* Low part of limit*/
	uint16_t limit_lo_part;

	/* Low and Medium part of base */
	uint16_t base_lo_part;
	uint8_t  base_mi_part;

	/* Access byte */
	uint8_t  access_byte;

	/* Flags and high part of limit */
	uint8_t  flags_and_limit_hi_part;

	/* High part of base */
	uint8_t  base_hi_part;

};

/* gdt_entry's field masks */
#define GDT_ENTRY_MASK_LIMIT_LO_PART   0xffff
#define GDT_ENTRY_MASK_LIMIT_HI_PART   0x0f0000

#define GDT_ENTRY_MASK_BASE_LO_PART    0x0000ffff
#define GDT_ENTRY_MASK_BASE_MI_PART    0x00ff0000
#define GDT_ENTRY_MAKS_BASE_HI_PART    0xff000000

/* gdt_entry's field shifts */
#define GDT_ENTRY_SHIFT_LIMIT_HI_PART  16
#define GDT_ENTRY_SHIFT_BASE_MI_PART   16
#define GDT_ENTRY_SHIFT_BASE_HI_PART   24
#define GDT_ENTRY_SHIFT_FLAGS          4



/* Access Byte definitions */
#define GDT_ENTRY_AB_DISABLED           (0b0  <<   7)
#define GDT_ENTRY_AB_PRESENT            (0b1  <<   7)

#define GDT_ENTRY_AB_DPL_RING3          (0b11 <<   5)
#define GDT_ENTRY_AB_DPL_RING2          (0b10 <<   5)
#define GDT_ENTRY_AB_DPL_RING1          (0b01 <<   5)
#define GDT_ENTRY_AB_DPL_RING0          (0b00 <<   5)

#define GDT_ENTRY_AB_TSS                (0b0  <<   4)
#define GDT_ENTRY_AB_CDSEG              (0b1  <<   4)

#define GDT_ENTRY_AB_CODE_SEGMENT       (0b1  <<   3)
#define GDT_ENTRY_AB_DATA_SEGMENT       (0b0  <<   3)

#define GDT_ENTRY_AB_DS_GROW_UP         (0b0  <<   2)
#define GDT_ENTRY_AB_DS_GROW_DOWN       (0b1  <<   2)

#define GDT_ENTRY_AB_CS_RING_MUST_MATCH (0b0  <<   2)
#define GDT_ENTRY_AB_CS_JLE             (0b1  <<   2)

#define GDT_ENTRY_AB_DS_UNWRITABLE      (0b0  <<   1)
#define GDT_ENTRY_AB_DS_WRITABLE        (0b1  <<   1)

#define GDT_ENTRY_AB_CS_UNREADABLE      (0b0  <<   1)
#define GDT_ENTRY_AB_CS_READABLE        (0b1  <<   1)

#define GDT_ENTRY_AB_NOT_ACCESSED       (0b0  <<   0)
#define GDT_ENTRY_AB_WAS_ACCESSED       (0b1  <<   0)


/* Flag definitions */
#define GDT_ENTRY_F_BYTE  (0b0 << 3)
#define GDT_ENTRY_F_KiB   (0b1 << 3)

#define GDT_ENTRY_F_16BIT   (0b0 << 2)
#define GDT_ENTRY_F_32BIT   (0b1 << 2)
#define GDT_ENTRY_F_CS64BIT (0b1 << 1)

/* Limit cannot be higher than 0xFFFFFF */
struct gdt_entry gdt_entry_create(uint32_t base, uint32_t limit, uint8_t flags, uint8_t access_byte);

/*
 * Since the GDT has to be at it's final location even during early boot,
 * it must be static in nature.
 */
#define GDT_MAX_ENTRIES 4

/*
 * This structure represents the global descriptor table itself.
 */
extern volatile struct gdt
{
	size_t count;
	struct gdt_entry entries[GDT_MAX_ENTRIES];

} kernel_gdt;

/*
 * This structure is what is eventually loaded into the gdtr register
 */
extern volatile struct __attribute__ ((aligned(2),packed)) gdt_location_info
{
	uint16_t limit;
	uint64_t base;
} kernel_gdt_gdtl;

/* This function sets the current segment registers */
void gdt_set_segment(uint64_t cs, uint64_t ds);

/* This function loads the GDT into the GDTR, while also setting the registers */
void gdt_load(uint64_t cs, uint64_t ds);

/* This function creates a new gdt, and replaces Limine's default one with it */
void gdt_init(void);

#define GDT_KERNEL_CS 0x8
#define GDT_KERNEL_DS 0x10

#endif // KERNEL_ARCH_AMD64_GDT_H_


