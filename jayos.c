#include "jayos.h"


#define PSR_MODE_SVC			0x12

typedef void (*os_task_func_type)( u32 );


os_tcb_type		*os_best_task;
os_tcb_type		*os_cur_task;
os_tcb_type		*os_task_ready_list;
os_tcb_type		*os_task_pending_list;
os_status_type	os_status;
u8				os_task_num;

//extern void os_schedule(void);

void os_task_init()
{
	os_best_task 			= NULL;
	os_cur_task				= NULL;
	os_task_ready_list		= NULL;
	os_task_pending_list	= NULL;
	os_status				= OS_STOP;
	os_task_num				= 0;
}

void os_task_start(void (*func)(u32 param), u32 param)
{
	func(param);

	while(1);  // XXX: should remove this task
}

void os_insert_task(os_tcb_type *new_tcb)
{
	os_tcb_type			*task_ptr = os_task_ready_list;

	if (task_ptr == NULL) {
		task_ptr = new_tcb;
		new_tcb->link.prev = NULL;
		new_tcb->link.next = NULL;
		os_best_task = new_tcb;
	} else {
		while (task_ptr->prio < new_tcb->prio) {
			if (task_ptr->link.next) 
				task_ptr = task_ptr->link.next;
			else {
				task_ptr->link.next = new_tcb;
				new_tcb->link.prev = NULL;
				return;
			}
		}
		new_tcb->link.prev = task_ptr->link.prev;
		new_tcb->link.next = task_ptr;
		task_ptr->link.prev->link.next = new_tcb;
		task_ptr->link.prev = new_tcb;
		if (os_best_task == task_ptr)
			os_best_task = new_tcb;
	}

	if (os_best_task == new_tcb && os_status == OS_RUN) {
		os_schedule();
	}
	
}

bool os_create_task(
	os_tcb_type				*p_tcb, 
	byte 					*p_stack, 
	u32		 				stack_size, 
	u32		 				prio,
	u32						slices,
	os_task_func_type 		p_task,
	u32						param)
{
	byte					*sp;
	os_context_frame_type  *cft;

	sp = (byte*)((u32)p_stack + stack_size * sizeof(u32));

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

	os_insert_task(p_tcb);

	os_task_num++;

	return TRUE;
}
