.global _bootup
_bootup:
		// Initialize SCTLR_EL2 and HCR_EL2 to save values before entering EL2.
		MSR SCTLR_EL2, XZR
		MSR HCR_EL2, XZR
	
		// Determine the EL2 Execution state.
		MRS X0, SCR_EL3
		ORR X0, X0, #(1<<10) // RW EL2 Execution state is AArch64.
		ORR X0, X0, #(1<<0) // NS EL1 is Non-secure world.
		MSR SCR_EL3, x0
		MOV X0, #0b01001 // DAIF=0000
		MSR SPSR_EL3, X0 // M[4:0]=01001 EL2h must match SCR_EL3.RW
		
		// Determine EL2 entry.
		ADR X0, el2_entry // el2_entry points to the first instruction of
		MSR ELR_EL3, X0 // EL2 code.
		ERET

el2_entry:
		// Initialize the SCTLR_EL1 register before entering EL1.
		MSR SCTLR_EL1, XZR

		// Determine the EL1 Execution state.
		MRS X0, HCR_EL2
		ORR X0, X0, #(1<<31) // RW=1 EL1 Execution state is AArch64.
		MSR HCR_EL2, X0
		MOV X0, #0b00101 // DAIF=0000
		MSR SPSR_EL2, X0 // M[4:0]=00101 EL1h must match HCR_EL2.RW.
		ADR X0, el1_entry // el1_entry points to the first instruction of
		MSR ELR_EL2, X0 // EL1 code.
		ERET

el1_entry:
		LDR	x1, = vectors
		MSR	vbar_el1, x1
	
_start:
		adrp x0, stack_top
		mov	sp, xo
		bl main

infinite_loop:	
		wfi
		b infinite_loop



// Need to remove below		
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
