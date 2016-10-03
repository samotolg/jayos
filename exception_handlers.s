.global	reset_handler
.global irq_handler

reset_handler:
		msr		cpsr_c, #0xc0|0x13	/* svc mode */
		ldr 	r1, =svc_stack_top
		ldr		sp, [r1]
		
		msr		cpsr_c, #0xc0|0x12	/* irq mode */
		ldr 	r1, =irq_stack_top;
		ldr		sp, [r1]
		
		msr 	cpsr_c, #0x00|0x13	/* back to svc mode and enable IRQ/FIQ */
		bl		main
				
irq_handler:
		sub  	lr, lr, #4
		stmfd	sp!, {lr}
		stmfd	sp!, {r0-r12, r14}
		mrs		r1, spsr
		stmfd	sp!, {r1}
		
		bl		irqHandler
		
		ldmfd	sp!, {r1}
		msr		spsr_cxsf, r1
		ldmfd	sp!, {r0-r12, r14}
		ldmfd	sp!, {pc}^
