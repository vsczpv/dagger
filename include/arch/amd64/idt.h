#ifndef KERNEL_ARCH_AMD64_IDT_H_
#define KERNEL_ARCH_AMD64_IDT_H_

#include <stdint.h>
#include <stddef.h>

/*
 * This struct represents a gate descriptor in the interrupt decriptor table.
 */
struct __attribute__ ((aligned(0x4),packed)) idt_entry
{

	/* Low part of ISR offset */
	uint16_t offset_lo_part;

	uint16_t selector;
	uint8_t  interrupt_stack_table_id;

	/* Gate type, DPL and present bit. */
	uint8_t  type_attributes;

	/* Higher parts of ISR offset */
	uint16_t offset_mi_part;
	uint32_t offset_hi_part;

	uint32_t padding;
};

/* idt_entry's field masks */
#define IDT_ENTRY_MASK_SEGMENT_SELECTOR  0xffff

#define IDT_ENTRY_MASK_OFFSET_LO_PART    0x000000000000ffff
#define IDT_ENTRY_MASK_OFFSET_MI_PART    0x00000000ffff0000
#define IDT_ENTRY_MASK_OFFSET_HI_PART    0xffffffff00000000

/* idt_entry's field shifts */
#define IDT_ENTRY_SHIFT_OFFSET_MI_PART   16
#define IDT_ENTRY_SHIFT_OFFSET_HI_PART   32

/* Interrupt Stack Table selectors */
#define IDT_ENTRY_ISTID_NONE             0x00
#define IDT_ENTRY_ISTID_IST1             0x01
#define IDT_ENTRY_ISTID_IST2             0x02
#define IDT_ENTRY_ISTID_IST3             0x03
#define IDT_ENTRY_ISTID_IST4             0x04
#define IDT_ENTRY_ISTID_IST5             0x05
#define IDT_ENTRY_ISTID_IST6             0x06
#define IDT_ENTRY_ISTID_IST7             0x07

/* Gate type definitions */
#define IDT_ENTRY_TA_GATE_TYPE_INTERRUPT 0xe
#define IDT_ENTRY_TA_GATE_TYPE_TRAP      0xf

/* Data protection level definitions */
#define IDT_ENTRY_TA_DPL_RING0           0b0000000
#define IDT_ENTRY_TA_DPL_RING1           0b0100000
#define IDT_ENTRY_TA_DPL_RING2           0b1000000
#define IDT_ENTRY_TA_DPL_RING3           0b1100000

/* Present bit */
#define IDT_ENTRY_TA_NOT_PRESENT         0b00000000
#define IDT_ENTRY_TA_PRESENT             0b10000000

struct idt_entry idt_entry_create(uint64_t isr_offset, uint16_t cs_selector, uint8_t istid, uint8_t gate_type, uint8_t dpl, uint8_t present);
struct idt_entry idt_entry_create_dummy(void);

/*
 * Just like the GDT, the IDT get's to be part of the BSS segment. */
#define IDT_MAX_ENTRIES 256

/*
 * This is the interrupt descriptor table itself.
 */
extern volatile __attribute__ ((aligned(0x4))) struct idt
{
	size_t count;
	struct idt_entry entries[IDT_MAX_ENTRIES];
} kernel_idt;

/*
 * This structure is what is eventually loaded into the idtr register
 */
extern volatile struct __attribute__ ((aligned(2),packed)) idt_location_info
{
	uint16_t limit;
	uint64_t base;
} kernel_idt_idtl;

void idt_load(void);
void idt_init(void);

#endif // KERNEL_ARCH_AMD64_IDT_H_
