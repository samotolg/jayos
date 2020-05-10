.global	reset_handler
.global irq_handler

.extern		__main
.extern		os_best_task
.extern		os_cur_task
.extern		os_status
.extern 	os_schedule_lock

#define OS_TCB_SIZE
#define OS_TCB_OFFSET_SP					0
#define OS_TCB_OFFSET_SP_LIMIT				(OS_TCB_OFFSET_SP + 4)
#define OS_TCB_OFFSET_SLICES				(OS_TCB_OFFSET_SP_LIMIT + 4)
#define OS_TCB_OFFSET_PRIO					(OS_TCB_OFFSET_SLICES + 4)
#define OS_TCB_OFFSET_SIGS					(OS_TCB_OFFSET_PRIO + 4)
#define OS_TCB_OFFSET_WAIT					(OS_TCB_OFFSET_WAIT+4)

#define OS_CONTEXT_OFFSET_SPSR				0
#define OS_CONTEXT_OFFSET_PC				0x3c

.equ			MODE_USER,			0x10
.equ			MODE_FIQ,			0x11
.equ			MODE_IRQ,			0x12
.equ			MODE_SVC,			0x13
.equ			MODE_ABT,			0x17
.equ			MODE_UNDEF,			0x1B

.equ			I_BIT,				(1 << 7)
.equ			F_BIT,				(1 << 6)
.equ			IF_BIT,				(0xC0)

.equ			CONTEXT_FRAME_SIZE,		0x40

.align			4
.text

reset_handler:
	ldr		r0, =MODE_SVC | I_BIT | F_BIT			@; SVC_MODE, I,F disable
	msr		CPSR_c, r0
	
	ldr		r0, =MODE_IRQ | I_BIT | F_BIT
	msr		CPSR_c, r0
	ldr		r13, =irq_stack_top

	ldr		r0, =MODE_FIQ | I_BIT | F_BIT
	msr		CPSR_c, r0
	ldr		r13, =exp_stack_top

	ldr		r0, =MODE_ABT | I_BIT | F_BIT
	msr		CPSR_c, r0
	ldr		r13, =exp_stack_top

	ldr		r0, =MODE_UNDEF| I_BIT | F_BIT
	msr		CPSR_c, r0
	ldr		r13, =exp_stack_top

	ldr		r0, =MODE_SVC| I_BIT | F_BIT
	msr		CPSR_c, r0
	ldr		r13, =svc_stack_top

@; modify exception vector address
@;	mrc		p15, 0, r0, c1, c0, 0
@;	orr		r0, #0x00002000
@;	mcr		p15, 0, r0, c1, c0, 0
	
@; CPU Cache Enable
@;	dsb
@;	mov		r0, #0x0
@;	mcr		p15, 0, r0, c7, c5, 0
@;	mcr		p15, 0, r0, c15, c5, 0
@;	isb
@;	mrc		p15, 0, r0, c1, c0, 0
@;	orr		r0, r0, #0x1 << 2
@;	orr		r0, r0, #0x1 << 12
@;	dsb
@;	mcr		p15, 0, r0, c1, c0, 0
@;	isb
	

	@; Variable Initialization
	bl		VariableInit

	bl		__main

VariableInit:
        ldr		r0, =__bss_start
        ldr		r1, =__bss_end
        mov		r2, #0x0
1:                                                                      @; Clear BSS(ZI) Region
        cmp		r0, r1
        strcc	r2, [r0], #4
        bcc		1b
        mov		pc, lr


    .global EnableIRQ
    .type   EnableIRQ, %function
EnableIRQ:
	mrs		 r0, CPSR
	bic		r0, r0, #I_BIT
	msr		CPSR_c, r0
	mov		r0, r0
	mov		r0, r0
	mov		r0, r0
	mov		pc, lr

    .global DisableIRQ
    .type   DisableIRQ, %function
DisableIRQ:
	mrs		r0, CPSR
	orr		r0, r0, #I_BIT
	msr		CPSR_c, r0
	mov		r0, r0
	mov		r0, r0
	mov		r0, r0
	mov		pc, lr

				
irq_handler:
	subs	lr, lr, #4
	stmfd	sp!, {lr}
	stmfd	sp!, {r0-r3, r12, lr}
	mrs		r1, spsr
	stmfd	sp!, {r1}
	
	bl		intr_irq_handler

/*------------------------------------------------------------------
; If OS has not been started, not context swithcing
;------------------------------------------------------------------*/
	ldr		r1, =os_status
	ldr		r1, [r1]
	cmp		r1, #0  				/* if (os_status == OS_STOP) */
	beq		task_swap_not_required	/* jump to task_swap_not_required */

/*------------------------------------------------------------------
; If schedule is locked, just return
;------------------------------------------------------------------*/
	ldr		a2, =os_schedule_lock
	ldr		a2, [a2]
	cmp		a2, #0
	bne		task_swap_not_required
	
/*------------------------------------------------------------------
; Check if task swap required
;------------------------------------------------------------------*/
	ldr     r1, =os_best_task
    ldr     r1, [r1]
    ldr     r3, =os_cur_task
    ldr     r0, [r3]
    cmp     r0, r1
    beq     task_swap_not_required

/*------------------------------------------------------------------
; Set the best task as current task
;------------------------------------------------------------------*/
/*	str		r1, [r3]			/* set the best task as current task */
/*	mov		r3, r0				/* the last task is a4. need to store context *
	
/*---------------------------------------------------------------------
; Restore the SPSR from stack.
;--------------------------------------------------------------------*/
    ldmfd     sp!, {r0}
    msr       spsr_c, r0
    msr       spsr_f, r0

/*---------------------------------------------------------------------
; Restore the registers from IRQ mode stack.
;--------------------------------------------------------------------*/
	ldmfd	sp!, {r0-r3, r12, lr}

/*------------------------------------------------------------------
; Save the context frame for outgoing task
;------------------------------------------------------------------*/
	msr		cpsr_c, #(MODE_SVC | IF_BIT)  /* change to supervisor mode to get right sp */
	sub		sp, sp, #4  		/* return address will be stored later: currently stored in irq stack */
	stmfd	sp!, {r0-r12, lr}	/* store r0 ~ r12, r14 */
	sub		sp, sp, #4			/* spsr will be stored later : currently stored in irq mode spsr */
	mov		r1, sp				/* stack address is completely made for outgoing task */

	ldr		r0, = os_cur_task	
	ldr		r2, [r0]
	str		r1, [r2, #OS_TCB_OFFSET_SP] /* store outgoing task's sp to the right position */

	msr		cpsr_c, #(MODE_IRQ | IF_BIT) /* change back to IRQ mode to get missing value */
	ldmfd	sp!, {r14}
	str 	r14, [r1, #OS_CONTEXT_OFFSET_PC] /* store return address for the outgoing task */
	mrs		r3, spsr
	str		r3, [r1, #OS_CONTEXT_OFFSET_SPSR] /* store spsr for the outging task */

	msr		cpsr_c, #(MODE_SVC | IF_BIT)  /* back to supervisor mode */
	
/*------------------------------------------------------------------
; Now restore incoming task's context frame
;------------------------------------------------------------------*/
	ldr		r1, =os_best_task
	ldr		r1, [r1]
	str		r1, [r0]		/* set os_curr_task to os_best_task */
	
	ldr		sp, [r1]
	ldmfd	sp!, {r0}
	msr		spsr_f, r0
	msr		spsr_c, r0
	mov		r1, sp
	add		sp, sp, #CONTEXT_FRAME_SIZE - 4
	ldmfd	r1, {r0-r12, r14, pc}^	/* restore registers */
	
task_swap_not_required:	
	ldmfd	sp!, {r1}
	msr		spsr_c, r1
	msr		spsr_f, r1
	ldmfd	sp!, {r0-r3, r12, lr}
	ldmfd	sp!, {pc}^
.end

		
