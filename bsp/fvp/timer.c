#include "stdio.h"
#include "stdint.h"
#include "aarch64.h"
#include "hal_interrupt.h"
#include "hal_timer.h"
#include "timer.h"

static void interrupt_handler(void);

#define TIMER_DEBUG 0

void hal_timer_init(void)
{
	debug_printf("[timer.c] hal_timer_init\n");

	// Enable UART
	uint32_t val;
	uint64_t ticks, current_cnt;

	// Disable the timer
	hal_timer_disable();

	// Set comparator value
	hal_timer_set_cval();

	// Enable the timer
	hal_timer_enable();

	// Register TIMER interrupt handler
	hal_interrupt_enable(TIMER_INTERRUPT);
	hal_interrupt_register_handler(interrupt_handler, TIMER_INTERRUPT);
}

void hal_timer_enable(void)
{
	// Enable the timer
	enable_cntv();

	if (TIMER_DEBUG)
	{
		uint32_t val;
		putstr("\t\tEnable the timer, CNTV_CTL_EL0 = ");
		val = raw_read_cntv_ctl();
		puthex(val);
		putstr("\n");
	}
    
}

void hal_timer_disable(void)
{
	// Disable the timer
	disable_cntv();

	if (TIMER_DEBUG)
	{
		uint32_t val;
		putstr("\t\tDisable the timer, CNTV_CTL_EL0 = ");
		val = raw_read_cntv_ctl();
		puthex(val);
		putstr("\n");
	}
}

void hal_timer_set_cval(void)
{
	uint64_t ticks, current_cnt;

	// Set comparator value
	cntfrq = raw_read_cntfrq_el0();

	// Next timer IRQ is after n sec(s).
	ticks = TIMER_WAIT * cntfrq;

	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();

	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);

	if (TIMER_DEBUG)
	{
		uint32_t val;

		// system frequency
		putstr("\n\t\tSystem Frequency: CNTFRQ_EL0 = ");
		puthex(cntfrq);

		// current timer value
		putstr("\n\t\tCurrent counter: CNTVCT_EL0 = ");
		puthex(current_cnt);

		debug_printf("\n\t\tAssert Timer IRQ after %u sec: CNTV_CVAL_EL0 = ", TIMER_WAIT);
		val = raw_read_cntv_cval_el0();
		puthex(val);
		putstr("\n");
	}
}

static void interrupt_handler(void)
{
	// Disable the timer
	hal_timer_disable();
	
	// Set comparator value
	hal_timer_set_cval();

	// Enable the timer
	hal_timer_enable();
}

/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
	Control register for the virtual timer.

	ISTATUS, bit [2]:	The status of the timer interrupt.
	IMASK, bit [1]:		Timer interrupt mask bit.
	ENABLE, bit [0]:	Enables the timer.
*/
uint32_t raw_read_cntv_ctl(void)
{
	uint32_t cntv_ctl;

	__asm__ __volatile__("mrs %0, CNTV_CTL_EL0\n\t" : "=r" (cntv_ctl) :  : "memory");
	return cntv_ctl;
}

void disable_cntv(void)
{
	uint32_t cntv_ctl;

	cntv_ctl = raw_read_cntv_ctl();
	cntv_ctl &= ~CNTV_CTL_ENABLE;
	__asm__ __volatile__("msr CNTV_CTL_EL0, %0\n\t" : : "r" (cntv_ctl) : "memory");
}

void enable_cntv(void)
{
	uint32_t cntv_ctl;

	cntv_ctl = raw_read_cntv_ctl();
	cntv_ctl |= CNTV_CTL_ENABLE;
	__asm__ __volatile__("msr CNTV_CTL_EL0, %0\n\t" : : "r" (cntv_ctl) : "memory");
}

/* CNTFRQ_EL0, Counter-timer Frequency register
	Holds the clock frequency of the system counter.
*/
uint32_t raw_read_cntfrq_el0(void)
{
	uint32_t cntfrq_el0;

	__asm__ __volatile__("mrs %0, CNTFRQ_EL0\n\t" : "=r" (cntfrq_el0) : : "memory");
	return cntfrq_el0;
}

void raw_write_cntfrq_el0(uint32_t cntfrq_el0)
{
	__asm__ __volatile__("msr CNTFRQ_EL0, %0\n\t" : : "r" (cntfrq_el0) : "memory");
}

/* CNTVCT_EL0, Counter-timer Virtual Count register
	Holds the 64-bit virtual count value.
*/
uint64_t raw_read_cntvct_el0(void)
{
	uint64_t cntvct_el0;

	__asm__ __volatile__("mrs %0, CNTVCT_EL0\n\t" : "=r" (cntvct_el0) : : "memory");
	return cntvct_el0;
}

/* CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register
	Holds the compare value for the virtual timer.
*/
uint64_t raw_read_cntv_cval_el0(void)
{
	uint64_t cntv_cval_el0;

	__asm__ __volatile__("mrs %0, CNTV_CVAL_EL0\n\t" : "=r" (cntv_cval_el0) : : "memory");
	return cntv_cval_el0;
}

void raw_write_cntv_cval_el0(uint64_t cntv_cval_el0)
{
	__asm__ __volatile__("msr CNTV_CVAL_EL0, %0\n\t" : : "r" (cntv_cval_el0) : "memory");
}
