ENTRY(start_kernel)
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ;
    rodata  PT_LOAD    FLAGS((1 << 2)) ;
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ;
}

SECTIONS
{

	. = 0xffffffff80000000;

	/DISCARD/ :
	{
		*(.rela*)
		*(.comment*)
		*(.eh_frame)
		*(.note .note.*)
	}

	/*
	.limine_reqs :
	{
		KEEP(*(.limine_reqs_array))
		KEEP(*(.limine_reqs_reqs))
	}
	*/

	.text : {
		*(.text .text.*)
	} :text

    /* Move to the next memory page for .rodata */
	. += CONSTANT(MAXPAGESIZE);

	.rodata : {
		*(.rodata .rodata*)
	} :rodata

    /* Move to the next memory page for .data */
	. += CONSTANT(MAXPAGESIZE);

	.data : {
		*(.data .data.*)
	} :data

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
	.bss : {
		*(COMMON)
		*(.bss .bss.*)
	} :data

	/*
	 * Even though .shstrtab is useless, most modern tools make it a pain in
	 * the ass to get rid of it, and it's not like I'm gonna use some hack
	 * with truncate to get rid of it (not that I've never done that before
	 * but that's exactly why I'm not doing it here, it's ugly).
	 */
	.shstrtab :
	{
		*(.shstrtab*)
	}

}
