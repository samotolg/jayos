.global _start
_start:
		mov r0, #0x00
		ldr r1, =interrupt_table
		ldr r3, =interrupt_table_end
keep_copying:
		ldr r2, [r1, #0x0]
		str r2, [r0, #0x0]
		add r0, r0, #0x4
		add r1, r1, #0x4
		cmp r1, r3
		bne keep_copying
		b 	reset_handler

infinite_loop:
		b infinite_loop

interrupt_table:
		ldr pc,	reset_handler_address
		ldr pc, undef_handler_address;
		ldr pc, swi_handler_address;
		ldr pc, prefetch_abort_handler_address;
		ldr pc, data_abort_handler_address;
		nop
		ldr pc, irq_handler_address
		ldr pc, fiq_handler_address;
	
reset_handler_address: 
		.word	reset_handler
undef_handler_address: 
		.word	infinite_loop
swi_handler_address:   
		.word	infinite_loop
prefetch_abort_handler_address:	
		.word	infinite_loop
data_abort_handler_address:	
		.word	infinite_loop
irq_handler_address: 
		.word	irq_handler
fiq_handler_address: 
		.word	infinite_loop

interrupt_table_end:	
