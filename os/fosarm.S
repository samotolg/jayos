.global	os_schedule	
.global	os_int_lock
.global os_int_free

.extern	os_best_task
.extern	os_cur_task
.extern os_schedule_lock

.equ	PSR_MODE_MASK,			0x1F
.equ	PSR_MODE_SVC,			0x13
.equ	PSR_MODE_IRQ,			0x12
.equ	PSR_IRQ_MASK,			0x80
.equ	PSR_FIQ_MASK,			0x40
.equ	CONTEXT_FRAME_SIZE,		0x40
.equ	PSR_IF_BIT_MASK,		0xC0

#define PSR_IRQ_MASK			0x80
#define PSR_FIQ_MASK			0x40



	.EQU	DAIF_DBG_BIT		(1<<3)
	.EQU	DAIF_ABT_BIT		(1<<2)
	.EQU	DAIF_IRQ_BIT		(1<<1)
	.EQU	DAIF_FIQ_BIT		(1<<0)

.align	4
.text

os_schedule:
/*------------------------------------------------------------------
    Disable IRQ/FIQ
------------------------------------------------------------------*/
	msr		DAIFSet, #DAIF_IRQ_BIT | #DAIF_FIQ_BIT

/*------------------------------------------------------------------
; If not in supervisor mode, just return
;------------------------------------------------------------------*/
	//and 	a1, a3, #PSR_MODE_MASK
	//cmp		a1, #PSR_MODE_SVC
	//bne		os_schedule_exit

/*------------------------------------------------------------------
; If schedule is locked, just return
;------------------------------------------------------------------*/
	ldr		w2, =os_schedule_lock
	ldr		w2, [w2]
	cmp		a2, #0
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
	mov		x4, x1				/* the last task is a4. need to store context *

/*------------------------------------------------------------------
; Save the context frame
;------------------------------------------------------------------*/
	stmfd	sp!, {lr}				/* store return address */
	sub		sp, sp, #8			/* r12, r14 are not reserved after function call */
	stmfd	sp!, {r4-r11}			/* store r4 ~ r11 */
	sub		sp, sp, #16			/* r0 ~ r3 are not reserved after function call */
	stmfd	sp!, {a3}				/* CPSR is stored in a3 */
	str		sp, [a4]			/* Finally store stack pointer */

/*------------------------------------------------------------------
; Perform context switching
;------------------------------------------------------------------*/
	mov		a1, a2				/* a1 is best task and need to be run */
	ldr		sp, [a1]			/* stack pointer is changed to new task */
	ldmfd	sp!, {a1}				/* a1 holds SPSR */
	msr		SPSR_cxf, a1			/* restore SPSR value */
	mov		a1, sp
	add		sp, sp, #CONTEXT_FRAME_SIZE - 4
	ldmfd	a1, {r0-r12, r14, pc}^	/* restore registers */

os_schedule_exit:
/*------------------------------------------------------------------
    Enable IRQ/FIQ and schedule return without context switching
------------------------------------------------------------------*/
	msr 	DAIFClr, #DAIF_IRQ_BIT | #DAIF_FIQ_BIT
	ret
	


os_schedule:
/*------------------------------------------------------------------
   Save CPSR and disable IRQ/FIQ
------------------------------------------------------------------*/
	mrs		a3, CPSR
	orr		a1, a3, #PSR_IF_BIT_MASK
	msr		CPSR_c, a1

/*------------------------------------------------------------------
; If not in supervisor mode, just return
;------------------------------------------------------------------*/
	and 	a1, a3, #PSR_MODE_MASK
	cmp		a1, #PSR_MODE_SVC
	bne		os_schedule_exit

/*------------------------------------------------------------------
; If schedule is locked, just return
;------------------------------------------------------------------*/
	ldr		a2, =os_schedule_lock
	ldr		a2, [a2]
	cmp		a2, #0
	bne		os_schedule_exit

/*------------------------------------------------------------------
; If os_best_task = os_cur_task, just return
;------------------------------------------------------------------*/
	ldr		a2, =os_best_task
	ldr		a2, [a2]
	ldr		a4, =os_cur_task
	ldr		a1, [a4]
	cmp		a2, a1
	beq		os_schedule_exit

/*------------------------------------------------------------------
; Set the best task as current task
;------------------------------------------------------------------*/
	str		a2, [a4]			/* set the best task as current task */
	mov		a4, a1				/* the last task is a4. need to store context *
	

/*------------------------------------------------------------------
; Save the context frame
;------------------------------------------------------------------*/
	stmfd	sp!, {lr}				/* store return address */
	sub		sp, sp, #8			/* r12, r14 are not reserved after function call */
	stmfd	sp!, {r4-r11}			/* store r4 ~ r11 */
	sub		sp, sp, #16			/* r0 ~ r3 are not reserved after function call */
	stmfd	sp!, {a3}				/* CPSR is stored in a3 */
	str		sp, [a4]			/* Finally store stack pointer */

/*------------------------------------------------------------------
; Perform context switching
;------------------------------------------------------------------*/
	mov		a1, a2				/* a1 is best task and need to be run */
	ldr		sp, [a1]			/* stack pointer is changed to new task */
	ldmfd	sp!, {a1}				/* a1 holds SPSR */
	msr		SPSR_cxf, a1			/* restore SPSR value */
	mov		a1, sp
	add		sp, sp, #CONTEXT_FRAME_SIZE - 4
	ldmfd	a1, {r0-r12, r14, pc}^	/* restore registers */

/*------------------------------------------------------------------
; Schedule return without context switching
;------------------------------------------------------------------*/
os_schedule_exit:
	msr		CPSR_f, a3
	msr		CPSR_c, a3
	bx		lr

os_int_lock:
	mrs		a1, cpsr
	orr		a2, a1, #PSR_IRQ_MASK
	msr		cpsr_c, a2
	and		a1, a1, #PSR_IRQ_MASK	/* if irq or fiq is locked, return none zero value */
	bx		lr

os_int_free:
	mrs		a1, cpsr
	bic		a2, a1, #PSR_IRQ_MASK
	msr		cpsr_c, a2
	and		a1, a1, #PSR_IRQ_MASK
	bx		lr
	
.end	
