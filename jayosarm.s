.equ	PSR_MODE_MASK,			0x1F
.equ	PSR_MODE_SVC,			0x12
/*.equ	PSR_IRQ_MASK,			0x80
.equ	PSR_FIQ_MASK,			0x40*/
.equ	CONTEXT_FRAME_SIZE,		0x40
.equ	PSR_IF_BIT_MASK,		0xC0

#define PSR_IRQ_MASK			0x80
#define PSR_FIQ_MASK			0x40


.global	os_schedule	
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
	beq		os_schedule_exit

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
	push	{lr}				/* store return address */
	sub		sp, sp, #8			/* r12, r14 are not reserved after function call */
	push	{r4-r11}			/* store r4 ~ r11 */
	sub		sp, sp, #16			/* r0 ~ r3 are not reserved after function call */
	push	{a3}				/* CPSR is stored in a3 */
	str		sp, [a4]			/* Finally store stack pointer */

/*------------------------------------------------------------------
; Perform context switching
;------------------------------------------------------------------*/
	mov		a1, a2				/* a1 is best task and need to be run */
	ldr		sp, [a1]			/* stack pointer is changed to new task */
	pop		{a1}				/* a1 holds SPSR */
	msr		SPSR_f, a1			/* restore SPSR value */
	msr		SPSR_c, a1
	mov		a1, sp				
	add		sp, sp, #CONTEXT_FRAME_SIZE-4 /* adjust stack pointer */
	ldmfd	sp!, {r0-r12, r14, pc}^	/* restore registers */

/*------------------------------------------------------------------
; Schedule return without context switching
;------------------------------------------------------------------*/
os_schedule_exit:
	msr		CPSR_f, a3
	msr		CPSR_c, a3
	