#include "bsp.h"
#include "rtos_type.h"
#include "fos.h"
#include "fos_timer.h"

extern void task_init();

/******************************************************************************

   This is the basic format to create a task.
   
   1. Need to define the following
     - stack size
     - priority of the task
     - slice time (When there are taks with same priority, 
                    OS will run in round robin way)
   2. Need to allocate memory for
     - stack (with stack size defined above)
     - tcb (task control block)

******************************************************************************/

#define IDLE_STACK_SIZE					  	1024
#define OS_TASK_PRIO_IDLE					255
#define OS_TASK_SLICE_IDLE					1000

static u32									idle_stack[IDLE_STACK_SIZE];
static os_tcb_type 							idle_tcb;

void idle_task(u32 param) 
{
	while(1);
}

/*****************************************************************************/


void __main(void)
{
	/* init uart */
//	uart_init();

	/* GIC inititialize */
//	intr_distributor_init();        
//	intr_interface_init();
//	EnableIRQ();

	os_init();
	printf("OS initialized!\n");
	
	/* Since the OS is built on the premise that there is an idle task 
	   with lowest priority, it is mandatory to create it. */
	os_create_task(
			&idle_tcb, 				/* pointer to the tcb 				*/
			idle_stack, 			/* pointer to the stack memory		*/
			IDLE_STACK_SIZE, 		/* stack size						*/
			OS_TASK_PRIO_IDLE,		/* task priority					*/
			OS_TASK_SLICE_IDLE, 	/* slice time for round robin		*/
			idle_task, 				/* function pointer	to the task		*/
			0, 						/* parameter for the task function  */
			"idle task");			/* task name used for debugging		*/

	/* All the other tasks should be created in the following function. */
	task_init();

	/* This function will not return */
	os_start();
	printf("OS started!\n");

	while (1); /* Control *MUST NOT* be here */
}

