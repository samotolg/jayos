#include "bsp.h"
#include "fos.h"
#include "fos_timer.h"

#define PSR_MODE_SVC						0x153			// extension field to be set as 1
#define OS_TIMER_MAX						10


/* If the following is defined, debug messages will come out thru serial port */

#define OS_SIG_TIMER						0x80000000

/******************************************************************************
                   Global Variables Declaration used in OS
******************************************************************************/
os_tcb_type									*os_best_task;
os_tcb_type									*os_cur_task;
os_tcb_type									*os_task_ready_list;
os_status_type								os_status;
u8											os_task_num;
os_timer_type								*os_active_timer_list;
u8											os_active_timer_count;
os_timer_type								*os_idle_timer_list;
os_timer_type								os_timer[OS_TIMER_MAX];
u32											os_timer_count;
u32											os_schedule_lock;
#ifdef OS_SCHEDULE_ROUND_ROBIN
bool										os_enable_rr;
#endif
u32											os_err_count;

/******************************************************************************
                      Function Prototype Declaration
******************************************************************************/
void 			os_timer_expired(int param);
void 			os_switching_rr(int param);
void 			os_set_best_task();


/******************************************************************************
 Function : os_init
 
 Description :
 	This function should be called before calling any os API.
 	It will initialze all the variables and status used in os.
 
******************************************************************************/
void os_init()
{
	int 	i;
	
	os_best_task 						= NULL;
	os_cur_task							= NULL;
	os_task_ready_list					= NULL;
	os_status							= OS_STOP;
	os_task_num							= 0;

	// timer initialization
	os_active_timer_list				= NULL;
	os_timer[0].prev					= NULL;
	os_timer[0].next					= &os_timer[1];
	os_timer[OS_TIMER_MAX-1].prev 		= &os_timer[OS_TIMER_MAX-2];
	os_timer[OS_TIMER_MAX-1].next 		= NULL;
	for (i=1;i<OS_TIMER_MAX-1;i++) {
		os_timer[i].prev = &os_timer[i-1];
		os_timer[i].next = &os_timer[i+1];
	}
	os_idle_timer_list					= &os_timer[0];
	os_active_timer_count				= 0;
	os_timer_init();
	os_timer_count						= 0;
	os_schedule_lock					= 0;
#ifdef OS_SCHEDULE_ROUND_ROBIN
	os_enable_rr						= FALSE;
#endif
		
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
	os_timer_start();
	
	os_set_best_task();
#ifdef OS_SCHEDULE_ROUND_ROBIN
	if ((!os_enable_rr && os_best_task->rr_link.next)) {
#ifdef	OS_DEBUG_TIMER
		printf("[TIM] RR Timer enabled!\n");
#endif
		os_hw_timer_enable(OS_ROUND_ROBIN_CHANNEL, os_best_task->slices, os_switching_rr);
		os_enable_rr = TRUE;
	}
	else {
		if (os_enable_rr && !(os_best_task->rr_link.next)) {
			os_hw_timer_disable(OS_ROUND_ROBIN_CHANNEL);
#ifdef	OS_DEBUG_TIMER
			printf("[TIM] RR Timer disabled!\n");
#endif			
			os_enable_rr = FALSE;
		}
	}
#endif
	os_schedule();

	while(1);
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

	/* XXX: If a task finishes its job, it will come to here. */
	/* Need to implement removing the task here later.        */
	while(1);  
}

/******************************************************************************
 Function : os_insert_to_ready_list (internal)
 
 Description :
 	This function will insert the new_tcb to the ready list. The list is 
 	sorted by priority and this function will place the new_tcb to the 
 	right position in terms of priority.  
 	XXX: This function should be re-considered when round robin is implemented.
 	 
******************************************************************************/
void os_insert_to_ready_list(os_tcb_type *new_tcb)
{
	os_tcb_type			*task_ptr = os_task_ready_list;

	OS_INTLOCK();
//	printf("INSERT TO READY LIST : %s\n", new_tcb->name);
	if (task_ptr == NULL) {
		/* XXX: Since there is always an idle task in the main list, 
		   control should not come here except inserting idle_task   */
		task_ptr = new_tcb;
		new_tcb->link.prev = NULL;
		new_tcb->link.next = NULL;
#ifdef OS_SCHEDULE_ROUND_ROBIN
		new_tcb->rr_link.prev = NULL;
		new_tcb->rr_link.next = NULL;
#endif
		os_task_ready_list = new_tcb;
	} else {
		while (task_ptr->prio < new_tcb->prio) { 
			/* try to find a task that has lower priority than new task */
			if (task_ptr->link.next) /* XXX: Need to confirm that link field has been initialized to null before hand*/
				task_ptr = task_ptr->link.next;
			else { 
				/* XXX: Since there is always an idle task in the main list, control should not come here */
				task_ptr->link.next = new_tcb;
				new_tcb->link.prev = task_ptr;
				new_tcb->link.next = NULL;
#ifdef OS_SCHEDULE_ROUND_ROBIN
				new_tcb->rr_link.prev = NULL;
				new_tcb->rr_link.next = NULL;
#endif				
				goto list_exit1;
			}
		}
#ifdef OS_SCHEDULE_ROUND_ROBIN
		if (task_ptr->prio == new_tcb->prio) {
			/* If there is a task that has same priority with the new task's, 
			   this task should be inserted into the round robin list */
			if (task_ptr->rr_link.next == NULL) { 
				/* start round robin scheduling */
				task_ptr->rr_link.next = new_tcb;
				task_ptr->rr_link.prev = new_tcb;
				new_tcb->rr_link.next = task_ptr;
				new_tcb->rr_link.prev = task_ptr;
			} else { 
				/* in case of already round robin, just place new task to the end of round robin */
				task_ptr->rr_link.prev->rr_link.next = new_tcb;
				new_tcb->rr_link.prev = task_ptr->rr_link.prev;
				new_tcb->rr_link.next = task_ptr;
				task_ptr->rr_link.prev = new_tcb;
			}
		} else {
			/* If there's no task that has same priority with the new task, round robin link will be nullified */
			new_tcb->rr_link.prev = NULL;
			new_tcb->rr_link.next = NULL;
#endif
			/* Insert the new task to the proper position in the main list */
			new_tcb->link.prev = task_ptr->link.prev;
			new_tcb->link.next = task_ptr;

			if (task_ptr->link.prev)
				task_ptr->link.prev->link.next = new_tcb;
			task_ptr->link.prev = new_tcb;

			if (os_task_ready_list->link.prev)
				os_task_ready_list = os_task_ready_list->link.prev;
			
#ifdef OS_SCHEDULE_ROUND_ROBIN
		}
#endif		
	}
list_exit1:	
	OS_INTFREE();
}

/******************************************************************************
 Function : os_remove_from_ready_list (internal)
 
 Description :
 	When a task waits a signal or receives the siganl that it was waiting,
 	the task will be removed from pending/ready list.
 	 
******************************************************************************/
void os_remove_from_ready_list(os_tcb_type *p_tcb)
{
#ifdef OS_SCHEDULE_ROUND_ROBIN
	os_tcb_type			*p_new_tcb;
#endif

	OS_INTLOCK();
//	printf("REMOVE FROM READY LIST : %s\n", p_tcb->name);

#ifdef OS_SCHEDULE_ROUND_ROBIN
	if (p_tcb->link.next == NULL) {  
		/* no other task having lower priority than removing task exists */
		/* Since there is an idle task which is always placed at the end of list, 
		   this is the case where a task in the round robin list is being removed. */
		if (p_tcb->rr_link.next) { 
			/* if this task is in the round robin list */
			if (p_tcb->rr_link.prev == p_tcb->rr_link.next)  { 
				/* only two tasks are running for round robin */
				/* by removing this task, round robin timer will be disabled in os_wait_sigs function */
				p_tcb->rr_link.prev->rr_link.next = NULL;
				p_tcb->rr_link.next->rr_link.prev = NULL;
			} else {
				/* More than two tasks are running in the round robin list */
				p_tcb->rr_link.prev->rr_link.next = p_tcb->rr_link.next;
				p_tcb->rr_link.next->rr_link.prev = p_tcb->rr_link.prev;
			}
		} else {
			/* XXX: Since there is an idle task which must be at the end of list, the control should not be here */
			printf("\n\n\n%%%%%%%%%%%%%%%%%%%%%% This is impossible4 %%%%%%%%%%%%%%%%%%%%%%%\n\n\n");
			printf("\nTask [%s] is not in the ready list\n", p_tcb->name);
			if (p_tcb->link.prev)
				p_tcb->link.prev->link.next = NULL;
		}

		/* Since link field is already null, only round robin link of the removing task is nullified */
		p_tcb->rr_link.prev = NULL;
		p_tcb->rr_link.next = NULL;
		return;
	}
#endif
#ifdef OS_SCHEDULE_ROUND_ROBIN
	if (p_tcb->rr_link.next) { 
		/* This is the case where the removing task is in the main list and also in the round robin list */
		if (p_tcb->rr_link.next == p_tcb->rr_link.prev) {  
			/* only two tasks are running for round robin */
			/* by removing this task, round robin timer will be disabled in os_wait_sigs function */
			p_tcb->rr_link.next->rr_link.next = NULL;
			p_tcb->rr_link.next->rr_link.prev = NULL;
		} else {
			/* More than two tasks are running in the round robin list */
			p_tcb->rr_link.next->rr_link.prev = p_tcb->rr_link.prev;
			p_tcb->rr_link.prev->rr_link.next = p_tcb->rr_link.next;
		}
		/* Since the removing task is the head of round robin list and also stay in the main list 
		   the next task(p_new_tcb here) in the round robin list will be replaced in the main list */ 
		p_new_tcb = p_tcb->rr_link.next;

		/* round robin link of the removing task is nullified and link field will be nullified later in this function */
		p_tcb->rr_link.prev = NULL;
		p_tcb->rr_link.next = NULL;		
	} else {
		p_new_tcb = NULL;
	}

#endif
	if (p_tcb->link.prev) { 
		/*  if the removing task is not the head of main list */
		if (p_tcb->link.next) { 
			/* Since there is an idle task, all the control should come 
			   to this way and the task is in the middle of list        */
#ifdef OS_SCHEDULE_ROUND_ROBIN		
			if (p_new_tcb) {
				/* The removing task is both in the main list and round robin list */
				/* replace p_tcb with the next task in the round robin list */
				p_new_tcb->link.next = p_tcb->link.next;
				p_new_tcb->link.prev = p_tcb->link.prev;
				p_tcb->link.prev->link.next = p_new_tcb;
				p_tcb->link.next->link.prev = p_new_tcb;
			} else {
				/* The removing task is only in the main list */
				p_tcb->link.prev->link.next = p_tcb->link.next;
				p_tcb->link.next->link.prev = p_tcb->link.prev;
			}
			p_tcb->link.prev = NULL;
			p_tcb->link.next = NULL;
#else			
			p_tcb->link.prev->link.next = p_tcb->link.next;
			p_tcb->link.next->link.prev = p_tcb->link.prev;
			p_tcb->link.prev = NULL;
			p_tcb->link.next = NULL;
#endif 			
		} else { 
			/* the removing task is the end of list */
			/* XXX: Since there is an idle task which must be at the end of list, 
			        the control should not be here. (idle task must not be removed at all) */
			printf("\n\n\n%%%%%%%%%%%%%%%%%%%%%% This is impossible3 %%%%%%%%%%%%%%%%%%%%%%%\n\n\n");
#ifdef OS_SCHEDULE_ROUND_ROBIN
			if (p_new_tcb) {
				/* The removing task is both in the main list and round robin list */
				/* replace p_tcb with the next task in the round robin list */
				p_new_tcb->link.next = NULL;
				p_new_tcb->link.prev = p_tcb->link.prev;
				p_tcb->link.prev->link.next = p_new_tcb;
			} else {
				p_tcb->link.prev->link.next = NULL;
			}
#else
			p_tcb->link.prev->link.next = NULL;
#endif
		}
	} else { 
		/* The removing task is the head of main list */
		if (p_tcb->link.next) { 
			/* if this task is in the round robin list */
#ifdef OS_SCHEDULE_ROUND_ROBIN
			if (p_new_tcb) {
				/* The removing task is both in the main list and round robin list */
				/* the round robin list has been already handled, only the main list will be re-tuned here */
				/* replace p_tcb with the next task in the main list */
				p_new_tcb->link.next = p_tcb->link.next;
				p_new_tcb->link.prev = NULL;
				p_tcb->link.next->link.prev = p_new_tcb;
				os_task_ready_list = p_new_tcb;
			} else {
				/* The removing task is only in the main list */
				p_tcb->link.next->link.prev = NULL;
				os_task_ready_list = p_tcb->link.next;
			}
			p_tcb->link.next = NULL;
#else
			p_tcb->link.next->link.prev = NULL;
			os_task_ready_list = p_tcb->link.next;
			p_tcb->link.next = NULL;
#endif			
		} else { 
			/* p_tcb is the only item in the list or this item is in the round robin list */
			/* XXX: Since there is an idle task which must be at the end of list, the control should not be here */
			printf("\n\n\n%%%%%%%%%%%%%%%%%%%%%% This is impossible2 %%%%%%%%%%%%%%%%%%%%%%%\n\n\n");
#ifdef OS_SCHEDULE_ROUND_ROBIN
			if (p_new_tcb) {
				p_new_tcb->link.next = NULL;
				p_new_tcb->link.prev = NULL;
				os_task_ready_list = p_new_tcb;
			} else {
				os_task_ready_list = NULL;
			}
#else		
			os_task_ready_list = NULL;
#endif
		}		
	}
	OS_INTFREE();
}


/******************************************************************************
 Function : os_create_task
 
 Description :
 	This function will be called by main function to create tasks.
 	 
******************************************************************************/
void os_create_task(
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
#ifdef OS_PRIORITY_INHERITANCE	
	p_tcb->prio_org			= 0;  // When priority inherited, this variable will hold the original priority
#endif	
	p_tcb->sigs				= NULL;
	p_tcb->wait				= NULL;
	p_tcb->suspend			= FALSE;

	p_tcb->name[TASK_NAME_MAX_LEN] = 0;
	if (name)
		while((p_tcb->name[pos] = name[pos]) && pos < TASK_NAME_MAX_LEN - 1) pos++;
#ifdef OS_SCHEDULE_ROUND_ROBIN
	p_tcb->rr_link.prev		= NULL;
	p_tcb->rr_link.next		= NULL;
#endif
#ifdef OS_SEMAPHORE
	p_tcb->wait_sem			= NULL;
	p_tcb->sem_link.prev	= NULL;
	p_tcb->sem_link.next 	= NULL;
#endif	
#ifdef OS_MUTEX
	p_tcb->wait_mutex 		= NULL;
	p_tcb->mutex_link.prev	= NULL;
	p_tcb->mutex_link.next	= NULL;
#endif	

	os_insert_to_ready_list(p_tcb);
	os_set_best_task();

	os_task_num++;

}

/******************************************************************************
 Function : os_set_best_task (internal)
 
 Description :
 	It will set the highest priority task among ready list to the os_best_task.
 	Before a running task goes to the suspend mode, the next task with highest
 	priority will be set to the os_best_task variable.
 	 
******************************************************************************/
void os_set_best_task()
{
	os_tcb_type		*p_task = os_task_ready_list;

#ifdef OS_SCHEDULE_ROUND_ROBIN
	while (p_task->suspend == TRUE) {
		if (p_task->rr_link.next) { 
			/* If the task has round robin list, we need to search in the round robin list */
			os_tcb_type *r_task;
			r_task = p_task->rr_link.next;
			while (r_task != p_task && r_task->suspend == TRUE)
				r_task = r_task->rr_link.next;
			if (r_task->suspend == FALSE) {
				os_best_task = r_task;
				return;
			}
		}
		p_task = p_task->link.next;
	}
#else
	while (p_task->suspend == TRUE)
		p_task = p_task->link.next;
#endif
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
	OS_INTLOCK();

	os_cur_task->wait |= sigs;
#ifdef OS_DEBUG_SIG
	printf("[SIG] [%s] task is waiting for [%p] sigs.\n", os_cur_task->name, os_cur_task->wait);
#endif
//	OS_INTLOCK();
	os_set_status(os_cur_task);
//	OS_INTFREE();
#ifdef OS_DEBUG_SIG
	printf("[SIG] [%s] task suspend is %u.\n", os_cur_task->name, os_cur_task->suspend);
#endif

	if (os_cur_task->suspend) {
		os_schedule();
	}

	sigs = os_cur_task->sigs;

	OS_INTFREE();
	
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

	OS_INTLOCK();
	
	old_sigs = p_tcb->sigs;
	p_tcb->sigs = old_sigs | sigs;

#ifdef OS_DEBUG_SIG
	printf("\n[SIG] [%s] received [%p] sigs.\n", p_tcb->name, p_tcb->sigs);
#endif

	p_tcb->wait = 0;  /* clear wait signal */
	os_set_status(p_tcb);
	
	if (p_tcb->suspend == FALSE) 
	{
		os_schedule();
	}

	OS_INTFREE();

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

	OS_INTLOCK();
	old_sigs = p_tcb->sigs;
	p_tcb->sigs = old_sigs & ~sigs;
	OS_INTFREE();

	return old_sigs;	
}

/******************************************************************************
 Function : os_get_cur_task (internal)
 
 Description :
 	It returns the task control block of the current task.
 	 
******************************************************************************/
os_tcb_type* os_get_cur_task()
{
	return os_cur_task;
}

/******************************************************************************
 Function : os_enable_timer (internal)
 
 Description :
 	When a user calls os_set_timer, os will call this function internally
 	to set the timer hardware specific timer.
 	 
******************************************************************************/
void os_enable_timer(u32 timer)
{
#ifdef	OS_DEBUG_TIMER
	printf("[TIM] OS Timer enabled!\n");
#endif
	os_hw_timer_enable(OS_TIMER_CHANNEL, timer, os_timer_expired);
}

/******************************************************************************
 Function : os_disable_timer (internal)
 
 Description :
 	This function will disable interrupt. But it does not actually stop
 	the hardware counter.
 	 
******************************************************************************/
void os_disable_timer()
{
	os_hw_timer_disable(OS_TIMER_CHANNEL);
#ifdef	OS_DEBUG_TIMER
	printf("[TIM] OS Timer disabled!\n");
#endif	
}

#ifdef OS_SCHEDULE_ROUND_ROBIN
/******************************************************************************
 Function : os_switching_rr (internal callback)
 
 Description :
 	This function will be called from IRQ context when the round robin timer
 	expires. OS needs to change the control of CPU to the next task in the 
 	round robin list.
 	 
******************************************************************************/
void os_switching_rr(int param)
{
	if (!os_schedule_lock) {
		if (os_best_task->rr_link.next) {
			os_best_task = os_best_task->rr_link.next;
			os_hw_timer_enable(OS_ROUND_ROBIN_CHANNEL, os_best_task->slices, os_switching_rr);
			os_schedule();
		} else {
			/* XXX: for some reason, round robin is not disabled. Will be disabled here. */
			os_hw_timer_disable(OS_ROUND_ROBIN_CHANNEL);
#ifdef	OS_DEBUG_TIMER
			printf("[TIM] RR Timer disabled! [XXX]\n");
#endif
		}
	}
	else {
#ifdef	OS_DEBUG_TIMER
//		printf("[TIM] os_schedule_locked! RR timer ignored.\n");
#endif
		os_hw_timer_enable(OS_ROUND_ROBIN_CHANNEL, os_best_task->slices, os_switching_rr);
	}
}
#endif


/******************************************************************************
 Function : os_enable_timer (internal callback)
 
 Description :
 	When the timer expires, this function is called in IRQ context.
 	This function will find the next nearest timer and set the timer with 
 	that value. If there's no other timer that is waiting, it will disable
 	the timer. Finally it will set the signals to the designated task.
 	 
******************************************************************************/
void os_timer_expired(int param)
{
	os_timer_type	*p_timer, expired_timer;

	p_timer = os_active_timer_list;
	if (p_timer == NULL)  // why this happens?  how we can disable the timer??
		return;
	
	os_active_timer_list = p_timer->next;
	
	expired_timer.p_tcb 	= p_timer->p_tcb;
	expired_timer.sigs  	= p_timer->sigs;
	os_active_timer_count--;

	p_timer->prev = NULL;
	p_timer->next = os_idle_timer_list;
	os_idle_timer_list = p_timer;

#ifdef OS_DEBUG_TIMER
	printf("[TIM] TIMER [%s(wait(%p):sigs(%p))] REMOVED!\n", expired_timer.p_tcb->name, expired_timer.p_tcb->wait, expired_timer.p_tcb->sigs);
#endif	
	os_set_sigs(expired_timer.p_tcb, expired_timer.sigs);

	if (os_active_timer_list) {
		os_active_timer_list->prev = NULL;
		os_enable_timer(os_active_timer_list->timer);
#ifdef OS_DEBUG_TIMER
		printf("[TIM] NEW ACTIVE TIMER HEAD IS [%s]\n", os_active_timer_list->p_tcb->name);
#endif
	}
	else {
#ifdef OS_DEBUG_TIMER
		printf("[TIM] TIMER EMPTY AND TIMER DISABLED\n", expired_timer.p_tcb->name);
#endif		
		os_disable_timer();
	}

}


/******************************************************************************
 Function : os_insert_timer_to_active_list (internal)
 
 Description :
 	Two timer lists are managed internally. One is active timer and the other
 	is idle timer. When a task sets a timer

 	XXX: When priority inheritance happens and a bug is detected. 
 	 
******************************************************************************/

void os_insert_timer_to_active_list(os_timer_type *new_timer)
{
	os_timer_type			*p_timer;
	u32						time_diff;
#ifdef OS_DEBUG_TIMER			
	u8						bPrint = FALSE;
#endif

	OS_INTLOCK();

	os_active_timer_count++;
	if (os_active_timer_list == NULL) {
		/* If there's no active timer, this is the first timer in the active list */
		os_active_timer_list = new_timer;
		new_timer->prev = new_timer->next = NULL;
		os_enable_timer(new_timer->timer);
#ifdef OS_DEBUG_TIMER		
		printf("\n[TIM] TIMER [%s(%u)] IS ADDED!\n", new_timer->p_tcb->name, new_timer->timer);
#endif		
		goto timer_list_exit;
	}

	p_timer = os_active_timer_list;

	/* Currently the hardware timer is counting the timer of the head of active timer. 
	   So this timer should be updated to the remaing time.				 */
	p_timer->timer = os_hw_timer_get_count(OS_TIMER_CHANNEL);

#if 1   // XXXXX: For debugging only.  Need to remove!!!
		os_timer_type  *debug_timer;
		debug_timer = p_timer;
		while (debug_timer) {
			if (debug_timer->p_tcb ==new_timer->p_tcb) {
				printf("\n\n\n\nDUPLICATED TIMER[%s] INSERTED!!!! Old(%u):New(%u)\n\n\n", new_timer->p_tcb->name, debug_timer->timer, new_timer->timer);
				os_err_count++;
				goto timer_list_exit;
				return;
			}
			debug_timer = debug_timer->next;
		}
#endif
	
	if (p_timer->timer >= new_timer->timer) {
		/* The new timer has shorter timer value than the timer in the head list */
		/* Need to place to the head of the active timer, and the previous timer should follow the new timer */
		p_timer->timer = p_timer->timer - new_timer->timer;  /* the previous timer value will be turned to relative value */
		new_timer->prev = NULL;
		new_timer->next = p_timer;
		p_timer->prev = new_timer;
		os_active_timer_list = new_timer;
		os_disable_timer();		/* XXX: Should we disable the previous timer before setting new value */
		os_enable_timer(new_timer->timer);
#ifdef OS_DEBUG_TIMER
		printf("\n[TIM] TIMER[%s] INSERTED TO THE HEAD!\n", new_timer->p_tcb->name);
		while (new_timer) {
			printf("--> %s(%u) ", new_timer->p_tcb->name, new_timer->timer);
			new_timer = new_timer->next;
		}
		printf("\n");
#endif

		goto timer_list_exit;
	}
	
#ifdef OS_DEBUG_TIMER	
	printf("\n[TIM] TIMER(%s) ADDED IN THE MIDDLE!\n", new_timer->p_tcb->name);
//	printf("%s(%u)", p_timer->p_tcb->name, p_timer->timer);
#endif

	/* Now the head of active timer has shorter timer value than new timer's.
	   New timer value should have relative timer value */
	time_diff = new_timer->timer - p_timer->timer;
	while (p_timer->next && (p_timer->next->timer < time_diff)) {
		/* try to find a task which has longer timer value than new timer */
		time_diff -= p_timer->next->timer;
#ifdef OS_DEBUG_TIMER			
		printf("--> %s(%u) ", p_timer->p_tcb->name, p_timer->timer);
		bPrint = TRUE;
#endif
		p_timer = p_timer->next;
	}

#ifdef OS_DEBUG_TIMER			
	if (!bPrint || (p_timer->next->timer >= time_diff))
		printf("--> %s(%u) ", p_timer->p_tcb->name, p_timer->timer);
#endif

	/* now the new timer should be placed between p_timer and p_timer->next. */
	/* if p_timer is the tail of the list, p_timer->next would be null       */
	new_timer->timer = time_diff;
	new_timer->next = p_timer->next;
	new_timer->prev = p_timer;
	if (p_timer->next)
		p_timer->next->prev = new_timer;
	p_timer->next = new_timer;

	if (new_timer->next) {
		/* The timer that should be just behind the new timer must recalculate
		   the timer value by subtracting the value of new timer.               */
		new_timer->next->timer -= new_timer->timer;
	}
	
#ifdef OS_DEBUG_TIMER
	while (new_timer) {
		printf("--> %s(%u) ", new_timer->p_tcb->name, new_timer->timer);
		new_timer = new_timer->next;
	}
	printf("\n");
#endif

timer_list_exit:
	OS_INTFREE();

}

/******************************************************************************
 Function : os_set_timer
 
 Description :
 	When the timer expires, this function is called in IRQ context.
 	This function will find the next nearest timer and set the timer with 
 	that value. If there's no other timer that is waiting, it will disable
 	the timer. Finally it will set the signals to the designated task.
 	 
******************************************************************************/
os_timer_type* os_set_timer(u32 timer, os_sig_type sigs)
{
	os_timer_type*	p_timer;

	OS_INTLOCK();  // Since idle_timer_list is used in os_timer_expired which is in IRQ context, we need to disable interrupt!
	p_timer = os_idle_timer_list;
	
	if (p_timer == NULL)
		return NULL;		/* no more timer available now */

	os_idle_timer_list = p_timer->next;
	if (os_idle_timer_list) os_idle_timer_list->prev = NULL;

	p_timer->p_tcb 		= os_get_cur_task();
	p_timer->timer		= timer;
	p_timer->sigs		= sigs;
	p_timer->prev		= NULL;
	p_timer->next		= NULL;

	os_insert_timer_to_active_list(p_timer);
	OS_INTFREE();
	
	return p_timer;
}

/******************************************************************************
 Function : os_delay
 
 Description :
 	When the timer expires, this function is called in IRQ context.
 	This function will find the next nearest timer and set the timer with 
 	that value. If there's no other timer that is waiting, it will disable
 	the timer. Finally it will set the signals to the designated task.
 	 
******************************************************************************/
void os_delay(u32 timer)
{
	os_timer_type*	p_timer;

	OS_INTLOCK();

	p_timer = os_idle_timer_list;
	if (p_timer == NULL)
		return;		/* no more timer available now */

	os_idle_timer_list = p_timer->next;
	if (os_idle_timer_list) os_idle_timer_list->prev = NULL;

	p_timer->p_tcb 			= os_get_cur_task();
	p_timer->p_tcb->wait 	|= OS_SIG_TIMER;
	p_timer->p_tcb->sigs 	&= ~(OS_SIG_TIMER);
	os_set_status(os_cur_task);
	
	p_timer->timer			= timer;
	p_timer->sigs			= OS_SIG_TIMER;
	p_timer->prev			= NULL;
	p_timer->next			= NULL;
	os_insert_timer_to_active_list(p_timer);

#ifdef OS_DEBUG_SIG
	printf("[SIG] [%s] task is waiting for [%p:%p] sigs {os_delay}\n", os_cur_task->name, os_cur_task->wait, os_cur_task->sigs);
#endif
	
	os_schedule();
	p_timer->p_tcb->wait &= ~(OS_SIG_TIMER);

#ifdef OS_DEBUG_SIG
	printf("[SIG] [%s] got [%p] sigs {in os_delay}\n", os_cur_task->name, os_cur_task->sigs);
#endif

	OS_INTFREE();
	
	return;
}


/******************************************************************************
 Function : os_set_status (internal)
 
 Description :
 	This function will check the event flags and semaphore status to decide
 	if the task is in suspend status or ready status. If there's any status
 	change, it will 

 	 
******************************************************************************/
void os_set_status(os_tcb_type *p_task)
{
	bool	old_suspend = p_task->suspend;
	os_tcb_type *p_pre_best;

	if (((p_task->wait & p_task->sigs) == p_task->wait) 	// there is no event signal the task is waiting
#ifdef OS_SEMAPHORE	
		&& (p_task->wait_sem == NULL) 						// there is no semaphore the task is waiting
#endif
#ifdef OS_MUTEX
		&& (p_task->wait_mutex == NULL) 					// there is no mutex the task is waiting
#endif
	) {
		p_task->suspend = FALSE;
		if (old_suspend != p_task->suspend)
			/* the task goes into the ready state from suspend state */
			os_insert_to_ready_list(p_task);
		else
			return;
	}
	else {
		p_task->suspend = TRUE;
		if (old_suspend != p_task->suspend)
			/* the task goes into the suspend state from ready state */
			os_remove_from_ready_list(p_task);
		else
			return;
	}

	/* control come over here only when the status has been changed */
	p_pre_best = os_best_task;
	os_set_best_task();
#ifdef OS_SCHEDULE_ROUND_ROBIN
	if ((!os_enable_rr && os_best_task->rr_link.next)) {
#ifdef	OS_DEBUG_TIMER
		printf("[TIM] RR Timer enabled!\n");
#endif			
		os_hw_timer_enable(OS_ROUND_ROBIN_CHANNEL, os_best_task->slices, os_switching_rr);
		os_enable_rr = TRUE;
	}
	else {
		if (os_enable_rr) {
			if (!(os_best_task->rr_link.next)) {
#ifdef	OS_DEBUG_TIMER
				printf("[TIM] RR Timer enabled!\n");
#endif				
				os_hw_timer_disable(OS_ROUND_ROBIN_CHANNEL);
				os_enable_rr = FALSE;
			} else if (p_pre_best != os_best_task) {
				/* Since the priority of best task has been changed, the round robin timer should be reset */
				os_hw_timer_enable(OS_ROUND_ROBIN_CHANNEL, os_best_task->slices, os_switching_rr);
				os_enable_rr = TRUE;
#ifdef	OS_DEBUG_TIMER
				printf("[TIM] RR Timer disabled!\n");
#endif				
			}
		} 
	}
#endif
}

