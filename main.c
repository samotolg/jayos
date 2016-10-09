#include "versatilepb.h"
#include "utils.h"
#include "os_types.h"
#include "fos.h"

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

/*****************************************************************************/
#define OS_SIGS_DUMMY				0x0001
#define OS_SIGS_MUMMY				0x0002

#define DUMMY_STACK_SIZE			1024
#define OS_TASK_PRIO_DUMMY			5
#define OS_TASK_SLICE_DUMMY			100

static os_tcb_type					dummy_tcb;
static byte							dummy_stack[DUMMY_STACK_SIZE];

void dummy_task(u32 param)
{
	int i;
	while(1) {
		print("dummy task!\n");
		for (i=0;i<100000000; i++);
		os_wait_sigs(OS_SIGS_DUMMY);
		os_clr_sigs(&dummy_tcb, OS_SIGS_DUMMY);
	}
}


/*****************************************************************************/
#define MUMMY_STACK_SIZE			1024
#define OS_TASK_PRIO_MUMMY			7
#define OS_TASK_SLICE_MUMMY			100

static os_tcb_type					mummy_tcb;
static byte							mummy_stack[MUMMY_STACK_SIZE];

void mummy_task(u32 param)
{
	int i;
	while(1) {
		print("mummy task!\n");
		for (i=0;i<100000000; i++);
		os_wait_sigs(OS_SIGS_MUMMY);
		os_clr_sigs(&mummy_tcb,OS_SIGS_MUMMY);
	}
}

/*****************************************************************************/
#define IDLE_STACK_SIZE				256
#define OS_TASK_PRIO_IDLE			255
#define OS_TASK_SLICE_IDLE			1000

static os_tcb_type					idle_tcb;
static byte							idle_stack[IDLE_STACK_SIZE];

void idle_task(u32 param)
{
	while(1) {
		print("idle Task!\n");
		os_set_sigs(&dummy_tcb, OS_SIGS_DUMMY);
		os_set_sigs(&mummy_tcb, OS_SIGS_MUMMY);
	}
}

/*****************************************************************************/

int main(void)
{
	os_timer_init();
//	os_timer_enable();
	
	print("Hello, World!\n");
	
	os_init();
	os_create_task(&idle_tcb, idle_stack, IDLE_STACK_SIZE, OS_TASK_PRIO_IDLE, OS_TASK_SLICE_IDLE, idle_task, 0, "idle_task");

	os_create_task(&dummy_tcb, dummy_stack, DUMMY_STACK_SIZE, OS_TASK_PRIO_DUMMY, OS_TASK_SLICE_DUMMY, dummy_task, 0, "dummy_task");
	os_create_task(&mummy_tcb, mummy_stack, MUMMY_STACK_SIZE, OS_TASK_PRIO_MUMMY, OS_TASK_SLICE_MUMMY, mummy_task, 0, "mummy_task");
	
	os_start();
	
	while(1);
	return 0;
}

void irqHandler()
{
	if (*(TIMER0 + TIMER_MIS)) {
		print("******************Timer Tick!!\n");
		*(TIMER0 + TIMER_INTCLR) = 1;
		os_timer_enable();
	}
	*(TIMER0 + TIMER_INTCLR) = 1;
}
