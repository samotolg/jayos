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
	uint32_t cntp_ctl;

	cntp_ctl = *CNTBASE_CNTP_CTL(0);
	
	if (cntp_ctl & CNTP_CTL_ISTATUS) {
		cntp_ctl &= ~CNTP_CTL_ISTATUS;
		os_timer_cb[0](1);
	}

	cntp_ctl = *CNTBASE_CNTP_CTL(1);
	if (*(volatile unsigned int *)TIMER_MIS(1)) {
		cntp_ctl &= ~CNTP_CTL_ISTATUS;
		os_timer_cb[1](1);
	}
}


void os_hw_timer_init()
{
	// No matter the RTOS runs in secure or nen-secure
	// In order to access the timer memory from Arm Development Stuido 
	// It seems to set the CNTNSAR (counter-timer access control register),
	// so that they can be seen. Even though we set it as 0xFF, it sets as 0x03
	// since it only has two timers.
	*(CNTCTL_CNTNSAR) = 0xFF;

	// Register TIMER interrupt handler
	for (int i=0; i<60; i++)
	{
		hal_interrupt_enable(i);//TIMER_INTERRUPT);
		hal_interrupt_register_handler(intr_irq_handler, i);//TIMER_INTERRUPT);
	}
	
	// CNTACR<n> provides the controls for frame CNTBaseN.
	for (int i=0; i<2; i++)
		*CNTCTL_CNTACR(i) = CNTACR_RPCT | CNTACR_RVCT | CNTACR_RFRQ | CNTACR_RWVT | CNTACR_RWPT;

	// Set Counter Control Register so that the counter is enabled and properly set frequency.
	*(REG_CNTCR) = (REG_CNT_FCREQ | REG_CNT_ENABLE);

	FVP_LED_ON(0)
	
}

u32 os_hw_timer_get_count(u8 ch)
{
	return *CNTBASE_CNTP_TVAL(ch);
}

void os_hw_timer_enable(u8 ch, u32 time, timer_cb_func_type func)
{
	uint32_t cntp_ctl;

	// Enable Counter<ch>
	cntp_ctl = *CNTBASE_CNTP_CTL(ch);
	cntp_ctl |= CNTP_CTL_ENABLE;
	*CNTBASE_CNTP_CTL(ch) = cntp_ctl;

	// Set Timer Value as time
	*CNTBASE_CNTP_TVAL(ch) = time;

	os_timer_cb[ch] = func;

	FVP_LED_ON(ch+1)
}

void os_hw_timer_disable(u8 ch)
{
	uint32_t cntp_ctl;

	// Disable Counter<ch>	
	cntp_ctl = *CNTBASE_CNTP_CTL(ch);
	cntp_ctl &= ~CNTP_CTL_ENABLE;
	*CNTBASE_CNTP_CTL(ch) = cntp_ctl;

	os_timer_cb[ch] = NULL;

	FVP_LED_OFF(ch+1)
}

void os_hw_timer_pause(u8 ch)
{
	uint32_t cntp_ctl;

	// Disable Counter<ch>	
	cntp_ctl = *CNTBASE_CNTP_CTL(ch);
	cntp_ctl &= ~CNTP_CTL_ENABLE;
	*CNTBASE_CNTP_CTL(ch) = cntp_ctl;	

	FVP_LED_ON(ch+3)
}

void os_hw_timer_resume(u8 ch)
{
	uint32_t cntp_ctl;

	// Enable Counter<ch>
	cntp_ctl = *CNTBASE_CNTP_CTL(ch);
	cntp_ctl |= CNTP_CTL_ENABLE;
	*CNTBASE_CNTP_CTL(ch) = cntp_ctl;

	FVP_LED_OFF(ch+3)
}

u32 os_hw_timer_get_global_count()
{
	return 1;
}
