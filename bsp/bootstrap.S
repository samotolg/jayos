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
		// Set up stack pointer for EL1
		adrp x0, stack_top
		mov	sp, x0

		mov x0, #(3 << 20)  // Enable floating point(library)
		msr CPACR_EL1, x0

		bl __main

infinite_loop:	
		wfi
		b infinite_loop

