#ifndef OS_SEM_H
#define OS_SEM_H

#include "fos.h"

#ifdef OS_SEMAPHORE

void os_sem_init(os_sem_type *sem, int count);
int os_sem_getvalue(os_sem_type *sem);
int os_sem_wait(os_sem_type *sem);
int os_sem_trywait(os_sem_type *sem);
int os_sem_post(os_sem_type *sem);

#endif
#endif
