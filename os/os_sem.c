#include "os_sem.h"

#ifdef OS_SEMAPHORE


/******************************************************************************
                      Function Prototype Declaration
******************************************************************************/
extern os_tcb_type* 	os_get_cur_task();
extern void 			os_set_best_task();

/******************************************************************************
 Function : os_sem_init
 
 Description :
 	This function will initialize the semaphore. The caller function needs to
 	have semaphore and give the memory address as a parameter. The count value
 	is defining the available resources.
 
******************************************************************************/
void os_sem_init(os_sem_type *sem, int count)
{
	OS_CRITICAL_SECTION_ENTER();
	sem->count 				= count;
	sem->list	 			= NULL;
	OS_CRITICAL_SECTION_EXIT();	
}

/******************************************************************************
 Function : os_sem_wait
 
 Description :
 	This function simply returns the current available resources. Since it 
 	does not lock the scheduling, the caller should not assume that the return 
 	value is still valid when it tries to wait the semaphore.
	
******************************************************************************/
int os_sem_getvalue(os_sem_type *sem)
{
	return sem->count;
}

/******************************************************************************
 Function : os_sem_wait
 
 Description :
 	If there's available resources, this function will count down and return
 	immediately. Otherwise, it will suspend the calling task until there's
 	available resources and the calling task is the highest priority to take it.
	Calling this function will cause the context switching if there's no 
	avaiable resource.
	
******************************************************************************/
int os_sem_wait(os_sem_type *sem)
{
	os_tcb_type *p_task, *p_cur_task, *p_prev = NULL;
	
	OS_CRITICAL_SECTION_ENTER();
	if (sem->count > 0) {
		sem->count--;
#ifdef OS_DEBUG_SEMAPHORE		
		printf("\n[SEM] [%s] takes the semaphore (%d)\n", os_get_cur_task()->name, sem->count);
#endif
		OS_CRITICAL_SECTION_EXIT();
		return sem->count;
	}

	p_cur_task = os_get_cur_task();	/* This is the task that called this function */
	if (p_cur_task->suspend == TRUE) {
		printf("\n\n[SEM] Abnormal case!!!!\n\n");
		OS_CRITICAL_SECTION_EXIT();
		return -1;
	}

	p_cur_task->wait_sem = sem;
	p_task = sem->list;				/* This is the highest task waiting the semaphore among the list */

#ifdef OS_DEBUG_SEMAPHORE		
	printf("\n[SEM] [%s] could not take the semaphore (%d). Will wait! \n", p_cur_task->name, sem->count);
#endif
	
	if (p_task == NULL) {
		/* There's no task waiting for this semaphore. Just place the task on the head of the list */
		sem->list = p_cur_task;
		os_set_status(p_cur_task);
		OS_CRITICAL_SECTION_EXIT();
		os_schedule();	
		return sem->count;
	}
	

	/* semaphore link should be maintained in ascending priority order. */
	while (p_task && p_task->prio <= p_cur_task->prio ) {
		if (p_task->sem_link.next == NULL)
			p_prev = p_task;
		p_task = p_task->sem_link.next;
	}

	/* Now the current task should be placed before p_task */
	/* If p_task is NULL, the new task should be placed at the tail of the list */
	p_cur_task->sem_link.next = p_task;
	if (p_task) {
		p_cur_task->sem_link.prev = p_task->sem_link.prev;
		if (p_task->sem_link.prev)
			p_task->sem_link.prev->sem_link.next = p_cur_task;
		p_task->sem_link.prev = p_cur_task;
	}
	else {
		p_cur_task->sem_link.prev = p_prev;
		p_prev->sem_link.next = p_cur_task;
	}

	if (p_task == sem->list) {
		/* current task should be placed at the head of list */
		sem->list = p_cur_task;
#ifdef OS_DEBUG_SEMAPHORE		
		printf("\n[SEM] [%s] placed at the head of the sem list. next task is [%s].\n", p_cur_task->name, p_task->name);
#endif		
	} else {
#ifdef OS_DEBUG_SEMAPHORE		
		printf("\n[SEM] [%s] placed behind [%s] in the sem list.\n", p_cur_task->name, p_cur_task->sem_link.prev->name);
#endif		
	}

	OS_INTLOCK();	
	os_set_status(p_cur_task);
	OS_INTFREE();
	OS_CRITICAL_SECTION_EXIT();
	os_schedule();
	return sem->count;
	
}


/******************************************************************************
 Function : os_sem_trywait
 
 Description :
 	If there are resources, count down the semaphore and return the remaining 
 	count. If there's no available resources, just return -1 without blocking.
 
******************************************************************************/
int os_sem_trywait(os_sem_type *sem)
{
	int ret_val;
	OS_CRITICAL_SECTION_ENTER();
	if (sem->count > 0) {
		sem->count--;
		ret_val = sem->count;
		return sem->count;
	} else 
		ret_val = -1;

	OS_CRITICAL_SECTION_EXIT();

	return ret_val;
}

/******************************************************************************
 Function : os_sem_post
 
 Description :
 	
******************************************************************************/

int os_sem_post(os_sem_type *sem)
{
	os_tcb_type		*p_task;
	
	OS_CRITICAL_SECTION_ENTER();

	if (sem->count == 0) {
		/* Check if there're some tasks waiting for this semaphore  */
		if (sem->list != NULL) {
			p_task = sem->list;
			if (sem->list->sem_link.next) {
				/* Make the next task in the list to the head */
				sem->list = sem->list->sem_link.next;
				sem->list->sem_link.prev = NULL;
			}
			else {
				/* No other tasks are waiting for this semaphore */
				sem->list = NULL;
			}
			p_task->wait_sem 		= NULL;
			p_task->sem_link.next 	= NULL;
			p_task->sem_link.prev 	= NULL;  // XXX:This should be alrady NULL.  So need to remove it later
#ifdef OS_DEBUG_SEMAPHORE
			printf("\n[SEM] [%s] releases the semaphore (%d) and [%s] will take it (%d)\n", os_get_cur_task()->name, sem->count, p_task->name, p_task->suspend);
#endif		
			os_set_status(p_task);
			OS_CRITICAL_SECTION_EXIT();
			os_schedule();
			return sem->count;
		} else {
			sem->count++;
		}
	} else {
		sem->count++;
	}

#ifdef OS_DEBUG_SEMAPHORE
	printf("\n[SEM] [%s] releases the semaphore (%d).\n", os_get_cur_task()->name, sem->count);
#endif
	OS_CRITICAL_SECTION_EXIT();
	return sem->count;
}


#endif // OS_SEMAPHORE
