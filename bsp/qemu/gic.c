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



