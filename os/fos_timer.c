#include "hw.h"
#include "fos_timer.h"


extern timer_cb_func_type 	os_timer_cb[2];

/*
* Description:
* Interrupt handler
*
* Remarks:
* Re-entrant interrupt handler
*
* Inputs:
* ack_register : Interrupt ID as read from the ACK register
* 			   by the low level interrupt handler
*
* Return Value:
* None
* 
*/
void intr_irq_handler(void)
{
	if (*(volatile unsigned int *)TIMER_MIS(0)) {
		*(volatile unsigned int *)TIMER_INTCLR(0) = 1;
		os_timer_cb[0](1);
	}

	if (*(volatile unsigned int *)TIMER_MIS(1)) {
		*(volatile unsigned int *)TIMER_INTCLR(1) = 1;
		os_timer_cb[1](1);
	}
}


void os_hw_timer_init()
{
	// Register TIMER interrupt handler
	hal_interrupt_enable(TIMER_INTERRUPT);
	hal_interrupt_register_handler(intr_irq_handler, TIMER_INTERRUPT);
	
	*(REG_CNTCR) = (REG_CNT_FCREQ | REG_CNT_ENABLE);
}

u32 os_hw_timer_get_count(u8 ch)
{
	return *(volatile unsigned int *)(TIMER_VALUE(ch));
}

void os_hw_timer_enable(u8 ch, u32 time, timer_cb_func_type func)
{
	*(volatile unsigned int *)(TIMER_LOAD(ch)) = time;
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
	os_timer_cb[ch] = func;
}

void os_hw_timer_disable(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_ONESHOT | TIMER_32BIT;
}

void os_hw_timer_pause(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_ONESHOT | TIMER_32BIT;
}

void os_hw_timer_resume(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
}

u32 os_hw_timer_get_global_count()
{
	return 1;
}
