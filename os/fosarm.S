.global	os_schedule	
.global	os_int_lock
.global os_int_free

.extern	os_best_task
.extern	os_cur_task
.extern os_schedule_lock

	.EQU	DAIF_DBG_BIT,		(1<<3)
	.EQU	DAIF_ABT_BIT,		(1<<2)
	.EQU	DAIF_IRQ_BIT,		(1<<1)
	.EQU	DAIF_FIQ_BIT,		(1<<0)

.align	8
.text

/*-------------------------------------------------------------------
    Save Context Prior to Context Switching
   Low  -------------
	    |  ELR_EL1  |
        -------------
	    |  SPSR_EL1 |
        -------------
	    |    X0     |
	    -------------
             ...   
             ...
	    -------------
	    |    X29    |
	    -------------
	    |  X30(LR)  |
        -------------
        |    XZR    |
   High -------------
--------------------------------------------------------------------*/
.macro save_context
	stp x30, xzr, [sp, #-16]!
	stp x28, x29, [sp, #-16]!
	stp x26, x27, [sp, #-16]!
	stp x24, x25, [sp, #-16]!
	stp x22, x23, [sp, #-16]!
	stp x20, x21, [sp, #-16]!
	stp x18, x19, [sp, #-16]!
	stp x16, x17, [sp, #-16]!
	stp x14, x15, [sp, #-16]!
	stp x12, x13, [sp, #-16]!
	stp x10, x11, [sp, #-16]!
	stp x8, x9, [sp, #-16]!
	stp x6, x7, [sp, #-16]!
	stp x4, x5, [sp, #-16]!
	stp x2, x3, [sp, #-16]!
	stp x0, x1, [sp, #-16]!

	mrs x0, elr_el1				/* save ELR_EL1						*/
	mrs x1, spsr_el1			/* save SPSR_EL1					*/
	stp x0, x1, [sp, #-16]!
.endm

.macro restore_context
	ldp x0, x1, [sp], #16
	msr elr_el1, x0				/* restore ELR_EL1                  */
	msr spsr_el1, x1			/* restore SPSR_EL1                 */
	ldp x0, x1, [sp], #16
	ldp x2, x3, [sp], #16
	ldp x4, x5, [sp], #16
	ldp x6, x7, [sp], #16
	ldp x8, x9, [sp], #16
	ldp x10, x11, [sp], #16
	ldp x12, x13, [sp], #16
	ldp x14, x15, [sp], #16
	ldp x16, x17, [sp], #16
	ldp x18, x19, [sp], #16
	ldp x20, x21, [sp], #16
	ldp x22, x23, [sp], #16
	ldp x24, x25, [sp], #16
	ldp x26, x27, [sp], #16
	ldp x28, x29, [sp], #16
	ldp x30, xzr, [sp], #16
.endm


os_schedule:
/*------------------------------------------------------------------
    Disable IRQ/FIQ
------------------------------------------------------------------*/
	msr		DAIFSet, #(DAIF_IRQ_BIT|DAIF_FIQ_BIT)

/*------------------------------------------------------------------
; If in IRQ mode, context switching is not allowed and just return
;------------------------------------------------------------------*/
	mrs 	x1, DAIF
	and		x1, x1, #DAIF_IRQ_BIT
	cmp		X1, #1
	beq		os_schedule_exit
	
/*------------------------------------------------------------------
; If schedule is locked, just return
;------------------------------------------------------------------*/
	ldr		x2, =os_schedule_lock
	ldr		x2, [x2]
	cmp		x2, #0
	bne		os_schedule_exit

/*------------------------------------------------------------------
; If os_best_task = os_cur_task, just return
;------------------------------------------------------------------*/
	ldr		x2, =os_best_task
	ldr		x2, [x2]
	ldr		x3, =os_cur_task
	ldr		x1, [x3]
	cmp		x2, x1
	beq		os_schedule_exit
	
/*------------------------------------------------------------------
; Set the best task as current task
;------------------------------------------------------------------*/
	str		x2, [x3]			/* set the best task as current task */
	mov		x4, x1				/* the last task is x4. need to store context *

/*------------------------------------------------------------------
; Save the context frame
;------------------------------------------------------------------*/
	save_context
	cmp 	x4, #0
	beq		context_switch
	mov		x1, sp
	str		x1, [x4]			/* save stack point in the last task */
	
/*------------------------------------------------------------------
; Perform context switching
;------------------------------------------------------------------*/
context_switch:
	mov		x1, x2				/* x1 is best task and need to be run */
	ldr		x3, [x1]			/* stack pointer is changed to new task */
	mov		sp, x3
	restore_context
	ret

os_schedule_exit:
/*------------------------------------------------------------------
    Enable IRQ/FIQ and schedule return without context switching
------------------------------------------------------------------*/
	msr 	DAIFClr, #(DAIF_IRQ_BIT|DAIF_FIQ_BIT)
	ret

os_int_lock:
/*------------------------------------------------------------------
    Disable IRQ and return the IRQ status prior to disabling IRQ
------------------------------------------------------------------*/
	mrs		x0, DAIF
	and		x0, x0, #DAIF_IRQ_BIT
	msr		DAIFSet,  #DAIF_IRQ_BIT
	ret

os_int_free:
/*------------------------------------------------------------------
    Enable IRQ and return the IRQ status prior to enabling IRQ
------------------------------------------------------------------*/
	mrs		x0, DAIF
	and		x0, x0, #DAIF_IRQ_BIT
	msr		DAIFClr, #DAIF_IRQ_BIT
	ret
