#include "hw.h"


/******************************************************************************
                      Stack Address
******************************************************************************/
#define SVC_STACK_SIZ				1024
#define IRQ_STACK_SIZ				1024
#define EXP_STACK_SIZ				1024

u32 svc_stack[SVC_STACK_SIZ];
u32 irq_stack[IRQ_STACK_SIZ];
u32	exp_stack[EXP_STACK_SIZ];

u32 *svc_stack_top = (u32 *)(svc_stack + (SVC_STACK_SIZ - 1) * 4);
u32 *irq_stack_top = (u32 *)(irq_stack + (IRQ_STACK_SIZ - 1) * 4);
u32 *exp_stack_top = (u32 *)(exp_stack + (EXP_STACK_SIZ - 1) * 4);

void sys_init()
{
#if 0
	uart_init();

	/* GIC inititialize */
	intr_distributor_init();        
	intr_interface_init();
	EnableIRQ();
#endif	
}
