#include "versatilepb.h"
#include "utils.h"

typedef unsigned int	uint32_t;

uint32_t	guard[4096];
uint32_t	svc_stack[4096];
uint32_t	irq_stack[4096];
uint32_t	guard2[4096];

void os_timer_init()
{
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
}

void os_timer_enable()
{
	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
}

int main(void)
{
	os_timer_init();
	os_timer_enable();
	
	print("Hello, World!\n");
	
	while(1);
	return 0;
}

void irqHandler()
{
	if (*(TIMER0 + TIMER_MIS)) {
		print("Timer Tick!!\n");
		*(TIMER0 + TIMER_INTCLR) = 1;
		os_timer_enable();
	}
	*(TIMER0 + TIMER_INTCLR) = 1;
}
