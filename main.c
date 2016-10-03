#include "versatilepb.h"
#include "utils.h"

typedef unsigned int	uint32_t;

uint32_t	svc_stack[4096];
uint32_t	irq_stack[4096];

void os_timer_enable();

int main(void)
{
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
	
	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
	
	print("Hello, World!\n");
	
	while(1);
	
	return 0;
}
void os_timer_set()
{
	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
}

void os_timer_enable()
{
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
	
	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
}



void irqHandler()
{
	print("Timer Tick!!!\n");
	if (*(TIMER0 + TIMER_VALUE) == 0) {
		print("Timer Tick!!\n");
		*(TIMER0 + TIMER_INTCLR) = 1;
		os_timer_enable();
	}
	*(TIMER0 + TIMER_INTCLR) = 1;
}

void callHandler()
{
	print("SHOULD NOT BE HERE!!!\n");
}


void testHandler()
{
	print("MUST NOT BE HERE!!!\n");
}

