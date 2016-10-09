#include "fos.h"

#define DISABLE_IRQ				__asm__ 	("msr	cpsr_c, 0xD3")
#define ENABLE_IRQ				__asm__		("msr	cpsr_c, 0x53")

#define PSR_MODE_SVC			0x113			// extension field to be set as 1

/******************************************************************************
                   Global Variables Declaration used in OS
******************************************************************************/
os_tcb_type						*os_best_task;
os_tcb_type						*os_cur_task;
os_tcb_type						*os_task_ready_list;
os_tcb_type						*os_task_pending_list;
os_status_type					os_status;
u8								os_task_num;

/******************************************************************************
                      Function Prototype Declaration
******************************************************************************/
extern void os_schedule();


/******************************************************************************
 Function : os_init
 
 Description :
 	This function should be called before calling any os API.
 	It will initialze all the variables and status used in os.
 
******************************************************************************/
void os_init()
{
	os_best_task 			= NULL;
	os_cur_task				= NULL;
	os_task_ready_list		= NULL;
	os_task_pending_list	= NULL;
	os_status				= OS_STOP;
	os_task_num				= 0;
}

/******************************************************************************
 Function : os_start
 
 Description :
 	After initializing and creating idle_task, this function could be called.
 	This will actually start the OS by scheduling and highest priorty task
 	will get the control.
 
******************************************************************************/
void os_start()
{
	os_status = OS_RUN;
	os_schedule();
}

/******************************************************************************
 Function : os_task_start
 
 Description :
 	This is the starting point for all tasks. When creating tasks, 
 	os designates this function to be called when it gets controls of CPU.
 	This function will start each task.
 
******************************************************************************/
void os_task_start(void (*func)(u32 param), u32 param)
{
	func(param);

	while(1);  // XXX: should remove this task
}

/******************************************************************************
 Function : os_insert_to_list
 
 Description :
 	This function will insert the new_tcb to the list. The list is sorted by
 	priority and this function will place the new_tcb to the right position
 	in terms of priority.  
 	XXX: This function should be re-considered when round robin is implemented.
 	 
******************************************************************************/
void os_insert_to_list(os_tcb_type **list, os_tcb_type *new_tcb)
{
	os_tcb_type			*task_ptr = *list;

	DISABLE_IRQ;
	if (task_ptr == NULL) {
		task_ptr = new_tcb;
		new_tcb->link.prev = NULL;
		new_tcb->link.next = NULL;
		*list = os_best_task = new_tcb;
	} else {
		while (task_ptr->prio < new_tcb->prio) {
			if (task_ptr->link.next) 
				task_ptr = task_ptr->link.next;
			else {
				task_ptr->link.next = new_tcb;
				new_tcb->link.prev = task_ptr;
				new_tcb->link.next = NULL;
				ENABLE_IRQ;
				return;
			}
		}
		new_tcb->link.prev = task_ptr->link.prev;
		new_tcb->link.next = task_ptr;
		task_ptr->link.prev->link.next = new_tcb;
		task_ptr->link.prev = new_tcb;
		if (os_best_task == task_ptr)
			os_best_task = new_tcb;
		if ((*list)->link.prev)
			*list = (*list)->link.prev;
	}

	ENABLE_IRQ;
}

/******************************************************************************
 Function : os_remove_from_list
 
 Description :
 	When a task waits a signal or receives the siganl that it was waiting,
 	the task will be removed from pending/ready list.
 	 
******************************************************************************/
void os_remove_from_list(os_tcb_type **list, os_tcb_type *p_tcb)
{
	DISABLE_IRQ;
	if (p_tcb->link.prev) { // p_tcb is not the head of list
		if (p_tcb->link.next) { // p_tcb is in the middle of list
			p_tcb->link.prev->link.next = p_tcb->link.next;
			p_tcb->link.next->link.prev = p_tcb->link.prev;
		} else { // p_tcb is the end of list
			p_tcb->link.prev->link.next = NULL;
		}
	} else { // p_tcb is the head of list
		if (p_tcb->link.next) { // p_tcb has the next task
			p_tcb->link.next->link.prev = NULL;
			*list = p_tcb->link.next; 	// change the list head
		} else { // p_tcb is the only item in the list
			*list = NULL;
		}		
	}
	ENABLE_IRQ;
}

/******************************************************************************
 Function : os_create_task
 
 Description :
 	This function will be called by main function to create tasks.
 	 
******************************************************************************/
bool os_create_task(
	os_tcb_type				*p_tcb, 
	os_stack_type			*p_stack, 
	u32		 				stack_size, 
	u32		 				prio,
	u32						slices,
	os_task_func_type 		p_task,
	u32						param,
	char					*name)
{
	os_stack_type			*sp;
	os_context_frame_type  *cft;
	short					pos = 0;

	sp = (u32*)((u32)p_stack + stack_size * sizeof(u32));

	sp -= sizeof(os_context_frame_type);

	cft 					= (os_context_frame_type*)sp;
	cft->spsr 				= PSR_MODE_SVC;
	cft->r[0] 				= (u32)p_task;
	cft->r[1] 				= param;
	cft->lr 				= (u32)os_task_start;

	p_tcb->sp				= sp;
	p_tcb->stack_limit		= p_stack;
	p_tcb->slices			= slices;
	p_tcb->prio				= prio;
	p_tcb->sigs				= NULL;
	p_tcb->wait				= NULL;
	p_tcb->suspend			= FALSE;

	p_tcb->name[TASK_NAME_MAX_LEN] = 0;
	if (name)
		while((p_tcb->name[pos] = name[pos]) && pos < TASK_NAME_MAX_LEN) pos++;

	os_insert_to_list(&os_task_ready_list, p_tcb);

	os_task_num++;

	return TRUE;
}


/******************************************************************************
 Function : os_set_best_task
 
 Description :
 	It will set the highest priority task among ready list to the os_best_task.
 	 
******************************************************************************/
void os_set_best_task()
{
	os_tcb_type		*p_task = os_task_ready_list;

	while (p_task->suspend == TRUE)  // XXX: Do we need this condition???
		p_task = p_task->link.next;
	os_best_task = p_task;
}

/******************************************************************************
 Function : os_wait_sigs
 
 Description :
 	When a task needs to wait some signals, it will call this function.
 	If the waiting signals are not ready, task swithcing will happen.
 	 
******************************************************************************/
os_sig_type os_wait_sigs(os_sig_type sigs)
{
	DISABLE_IRQ;
	if ((os_cur_task->sigs & sigs) == 0) {
		os_cur_task->suspend = TRUE;
		os_cur_task->wait |= sigs;
		os_remove_from_list(&os_task_ready_list, os_cur_task);
		os_insert_to_list(&os_task_pending_list, os_cur_task);
		os_set_best_task();
		os_schedule();
	}

	sigs = os_cur_task->sigs;

	ENABLE_IRQ;
	
	return sigs;	
}

/******************************************************************************
 Function : os_set_sigs
 
 Description :
 	This function will set the signals to the p_tcb task. If the signals are
 	that p_tcb was waiting for, and p_tcb task has higher priority then current
 	task, a task switching will happen.
 	 
******************************************************************************/
os_sig_type	os_set_sigs(os_tcb_type *p_tcb, os_sig_type sigs)
{
	os_sig_type		old_sigs;

	DISABLE_IRQ;
	
	old_sigs = p_tcb->sigs;
	p_tcb->sigs = old_sigs | sigs;

	if ((p_tcb->wait & sigs) == sigs)
	{
		p_tcb->suspend = FALSE;
		os_remove_from_list(&os_task_pending_list, p_tcb);
		os_insert_to_list(&os_task_ready_list, p_tcb);
		os_schedule();
	}

	ENABLE_IRQ;

	return old_sigs;
}

/******************************************************************************
 Function : os_clr_sigs
 
 Description :
 	It will clear the designated signals from the p_tcb. If a task was waiting
 	for the sigs and it got the signals, the task should clear the signals 
 	using this function.
 	 
******************************************************************************/
os_sig_type	os_clr_sigs(os_tcb_type *p_tcb, os_sig_type sigs)
{
	os_sig_type		old_sigs;

	DISABLE_IRQ;
	old_sigs = p_tcb->sigs;
	p_tcb->sigs = old_sigs & ~sigs;
	ENABLE_IRQ;

	return old_sigs;	
}

