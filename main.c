#include "versatilepb.h"
#include "utils.h"
#include "jayos_types.h"
#include "jayos.h"

#define SVC_STACK_SIZ				1024
#define IRQ_STACK_SIZ				1024

u32 svc_stack[SVC_STACK_SIZ];
u32 irq_stack[IRQ_STACK_SIZ];

u32 *svc_stack_top = (u32 *)(svc_stack + (SVC_STACK_SIZ - 1) * 4);
u32 *irq_stack_top = (u32 *)(irq_stack + (IRQ_STACK_SIZ - 1) * 4);

void os_timer_init(void)
{
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
}

void os_timer_enable(void)
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
