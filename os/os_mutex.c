#include "os_mutex.h"

#ifdef OS_MUTEX

static u32 os_mutex_count;

/******************************************************************************
                      Function Prototype Declaration
******************************************************************************/
extern os_tcb_type* 	os_get_cur_task();
extern void 			os_set_best_task();
extern void 			os_insert_to_ready_list(os_tcb_type *new_tcb);
extern void 			os_remove_from_ready_list(os_tcb_type *p_tcb);

/******************************************************************************
 Function : os_mutex_init
 
 Description :
 	This function will initialize the mutex. The caller function needs to
 	have mutex and give the memory address as a parameter.
 
******************************************************************************/
void os_mutex_init(os_mutex_type *mutex)
{
	OS_CRITICAL_SECTION_ENTER();
	mutex->list	 			= NULL;
	mutex->id				= ++os_mutex_count;
	OS_CRITICAL_SECTION_EXIT();	
}

/******************************************************************************
 Function : os_mutex_wait
 
 Description :
	If the mutex->list is NULL, this mutex is not owend by any task. Otherwise,
	this mutex is owned by the task pointed by mutex->list.
	
******************************************************************************/
int os_mutex_wait(os_mutex_type *mutex)
{
	os_tcb_type *p_task, *p_cur_task, *p_prev = NULL;

	OS_CRITICAL_SECTION_ENTER();

	p_cur_task = os_get_cur_task();	/* This is the task that called this function */

#ifdef OS_DEBUG_MUTEX	
	if (p_cur_task->wait) {
		printf("\n\n\n\n\n\n*** Why Process is going while waiting for other event ***\n");
	}
#endif

	if (mutex->list == NULL) {
		/* no other task owns this mutex */
		mutex->list = p_cur_task;
#ifdef OS_DEBUG_MUTEX		
		printf("\n[MTX] [%s] takes the mutex id (%d)\n", p_cur_task->name, mutex->id);
#endif
		OS_CRITICAL_SECTION_EXIT();
		return TRUE;
	}

	/* If the mutex is owned by other task, need to be in line in the list by priority descending order */
	p_task = mutex->list;
	p_cur_task->wait_mutex = mutex;

	while (p_task && p_task->prio <= p_cur_task->prio) {
		if (p_task->mutex_link.next == NULL)  		// New task should be placed at the tail of the list 
			p_prev = p_task;						// Remember the tail of the list before p_task turns to NULL
		p_task = p_task->mutex_link.next;
	}

	/* Now the current task should be placed before p_task */
	/* If p_task is NULL, the new task should be placed at the tail of the list */
	if (p_task) {
		if (p_task == mutex->list) {
			/* The priority of current mutex owner is lower than this task. Priority inversion happens */
			/* Need to place the incoming task at the head of the list */
			p_cur_task->mutex_link.prev = p_task;
			p_cur_task->mutex_link.next = p_task->mutex_link.next;
			if (p_task->mutex_link.next)
				p_task->mutex_link.next->mutex_link.prev = p_cur_task;
			p_task->mutex_link.next = p_cur_task;
#ifdef OS_PRIORITY_INHERITANCE	
			printf("\nPriority Inversion!!!! ");
			printf("[%s] is waiting for the task [%s] releasing mutex.", p_cur_task->name, p_task->name);
			printf("\n[%s:%d] placed at the head of the mutex list.\n", p_cur_task->name, p_cur_task->prio);
#endif

#ifdef OS_PRIORITY_INHERITANCE
			/* Priority Inheritance																	*/
			/* In order to avoid the mutex owner blocked by other lower priority than incoming task */
			/* the priority of the mutex owner is elevating as high as the incoming task			*/
			mutex->list->prio_org = mutex->list->prio;
			mutex->list->prio = p_cur_task->prio;

#ifdef OS_PRIORITY_INHERITANCE
			printf("Priority of [%s:%d] goes high from %d to %d\n", mutex->list->name, mutex->list->suspend, mutex->list->prio_org, mutex->list->prio);
#endif	
			/* XXX: Here we simply remove the task from ready list, and reinsert it again 
			   to place the right position in the ready list. This will cost a lot from 
			   performance perspective. Need to refine the algorithms for this later */ 
			/* XXXXX: There is a case that the task is waiting for some event, 
			   so we should not make it run without that signal. */
#if 0
			if (mutex->list->suspend == FALSE) {
				os_remove_from_ready_list(mutex->list);
				mutex->list->suspend = TRUE;
			}
#endif		
			OS_INTLOCK();
			os_set_status(p_cur_task);
			os_set_status(mutex->list);
			OS_INTFREE();
			OS_CRITICAL_SECTION_EXIT();
			os_schedule();
			return TRUE;
#endif
		} else {
			/* in case of the middle in the list */
			p_cur_task->mutex_link.next = p_task;
			p_cur_task->mutex_link.prev = p_task->mutex_link.prev;
			p_task->mutex_link.prev->mutex_link.next = p_cur_task;
			p_task->mutex_link.prev = p_cur_task;
#ifdef OS_DEBUG_MUTEX		
			printf("\n[MTX] [%s] placed in the middle of the mutex list. ", p_cur_task->name);
			printf("prev_task is [%s] and next task is [%s].\n", p_cur_task->mutex_link.prev->name, p_cur_task->mutex_link.next->name);
#endif				
		}
	}
	else {
		/* in case of the tail in the list*/
		p_cur_task->mutex_link.next = p_task;
		p_cur_task->mutex_link.prev = p_prev;
		p_prev->mutex_link.next = p_cur_task;
#ifdef OS_DEBUG_MUTEX		
		printf("\n[MTX] [%s] placed at the tail of the mutex list. prev_task is [%s].\n", 
				p_cur_task->name, p_cur_task->mutex_link.prev->name);
#endif			
	}

	OS_INTLOCK();
	os_set_status(p_cur_task);
	OS_INTFREE();
	OS_CRITICAL_SECTION_EXIT();
	os_schedule();
	return TRUE;
	
}

/******************************************************************************
 Function : os_mutex_post
 
 Description :
 	
******************************************************************************/
int os_mutex_post(os_mutex_type *mutex)
{
	os_tcb_type		*p_task, *p_cur_task;

	OS_CRITICAL_SECTION_ENTER();
	
	/* Only the mutex owner can post the mutex */
	p_cur_task = os_get_cur_task();
	if (mutex->list != p_cur_task) {
		OS_CRITICAL_SECTION_EXIT();
		return -1;
	}

	if (mutex->list->mutex_link.next == NULL) {
		/* no other task is waiting for this mutex */
#ifdef OS_DEBUG_MUTEX
		printf("\n[MTX] [%s] releases the mutex (%d)\n", mutex->list->name, mutex->id);
#endif	
		mutex->list = NULL;
		OS_CRITICAL_SECTION_EXIT();
		return TRUE;
	}

	/* Make the next task in the mutex list head of the list */
	p_task = p_cur_task->mutex_link.next;
	p_task->mutex_link.prev = NULL;
	mutex->list = p_task;
	
	p_cur_task->mutex_link.next = NULL;
	p_task->wait_mutex = NULL;

#ifdef OS_PRIORITY_INHERITANCE
	/* If the this task changed the priority for avoding priority inversion, 
	   the original priority is recovered here. */
	if (p_cur_task->prio_org != 0) {
#ifdef OS_PRIORITY_INHERITANCE
		printf("\nPriority of [%s] goes down from %d to %d\n", p_cur_task->name, p_cur_task->prio, p_cur_task->prio_org);
#endif	   
		p_cur_task->prio = p_cur_task->prio_org;
		p_cur_task->prio_org = 0;

		/* XXX: Here we simply remove the task from ready list, and reinsert it again 
		   to place the right position in the ready list. This will cost a lot from 
		   performance perspective. Need to refine the algorithms for this later */ 
		os_remove_from_ready_list(p_cur_task);
		os_insert_to_ready_list(p_cur_task);
		OS_INTLOCK();
		os_set_status(p_cur_task);
		OS_INTFREE();
	}
#endif

	p_cur_task->wait_mutex = NULL;
#ifdef OS_DEBUG_MUTEX
	printf("\n[MTX] [%s] releases the mutex and [%s] will take it (%d)\n", p_cur_task->name, p_task->name, mutex->id);
#endif			
	OS_INTLOCK();
	os_set_status(p_task);
	OS_INTFREE();
	OS_CRITICAL_SECTION_EXIT();
	os_schedule();

	return TRUE;
}


#endif // OS_MUTEX

