#include <arch/amd64/idt.h>
#include <arch/amd64/gdt.h>
#include <arch/amd64/interrupts.h>
#include <ktext.h>
#include <panic.h>

/* The interrupt descriptor table */
volatile struct idt               kernel_idt      = {0};
volatile struct idt_location_info kernel_idt_idtl = {0};

/* Create a dummy isr that just halts the kernel */
struct idt_entry idt_entry_create_dummy(void)
{
	return idt_entry_create
	(
		(uint64_t) isr_dummy_interrupt_entry,
		GDT_KERNEL_CS,
		IDT_ENTRY_ISTID_NONE,
		IDT_ENTRY_TA_GATE_TYPE_INTERRUPT,
		IDT_ENTRY_TA_DPL_RING0,
		IDT_ENTRY_TA_PRESENT
	);
}

struct idt_entry idt_entry_create(uint64_t isr_offset, uint16_t cs_selector, uint8_t istid, uint8_t gate_type, uint8_t dpl, uint8_t present)
{

	struct idt_entry entry = {0};

	entry.offset_lo_part =  isr_offset & IDT_ENTRY_MASK_OFFSET_LO_PART;
	entry.offset_mi_part = (isr_offset & IDT_ENTRY_MASK_OFFSET_MI_PART) >> IDT_ENTRY_SHIFT_OFFSET_MI_PART;
	entry.offset_hi_part = (isr_offset & IDT_ENTRY_MASK_OFFSET_HI_PART) >> IDT_ENTRY_SHIFT_OFFSET_HI_PART;

	entry.selector                 = cs_selector;
	entry.interrupt_stack_table_id = istid;

	entry.type_attributes  = gate_type;
	entry.type_attributes |= dpl;
	entry.type_attributes |= present;

	entry.padding = 0;

	return entry;
}

/* Execute the lidt instruction */
static void __attribute__ ((naked)) idt_load_execute_lidt(void)
{
	__asm__ volatile
	(
		"lidt (kernel_idt_idtl)\n"
		"ret"
		:::
	);
}

/* Load the IDT */
void idt_load()
{

	kernel_idt_idtl.base  = (uint64_t) &kernel_idt.entries;
	kernel_idt_idtl.limit = kernel_idt.count * sizeof (struct idt_entry) - 1;

	idt_load_execute_lidt();

	return;
}

void idt_init(void)
{

	/* Populate IDT with dummy ISRs */
	for (size_t i = 0; i < IDT_MAX_ENTRIES; i++)
		kernel_idt.entries[kernel_idt.count++] = idt_entry_create_dummy();

	/* Load idt */
	idt_load();

	kprintln("IDT successfully initialized.");

	return;
}