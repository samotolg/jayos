.global	reset_handler
.global irq_handler


.equ		PSR_SVC_MODE,		0x13
.equ		PSR_IRQ_MODE,		0x12

.equ		PSR_I_BIT,			0x80
.equ		PSR_F_BIT,			0x40
.equ		PSR_CLEAR,			0x00


reset_handler:
		msr		cpsr_c, PSR_I_BIT|PSR_F_BIT|PSR_SVC_MODE	/* svc mode */
		ldr 	r1, =svc_stack_top
		ldr		sp, [r1]
		
		msr		cpsr_c, PSR_I_BIT|PSR_F_BIT|PSR_IRQ_MODE	/* irq mode */
		ldr 	r1, =irq_stack_top;
		ldr		sp, [r1]
		
		msr 	cpsr_c, PSR_CLEAR|PSR_SVC_MODE	/* back to svc mode and enable IRQ/FIQ */
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
