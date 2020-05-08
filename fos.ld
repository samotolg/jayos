ENTRY(_bootup)

SECTIONS
{
	. = 0x06000000;
	. = ALIGN(4);
	__text_start = .;
	.text : { *(.text) }
	__text_end = .;
	
	. = ALIGN(4);
	__rodata_start = .;
	.rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
	__rodata_end = .;

	. = ALIGN(4);
	__data_start = .;
	.data : { *(.data) }
	__data_end = .;

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) }
	__bss_end = .;

	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
}
