#ifndef OS_SEM_H
#define OS_SEM_H
#ifdef OS_SEMAPHORE

#include "fos.h"

extern void os_sem_init(os_sem_type *sem, int count);
extern int os_sem_getvalue(os_sem_type *sem);
extern int os_sem_wait(os_sem_type *sem);
extern int os_sem_trywait(os_sem_type *sem);
extern int os_sem_post(os_sem_type *sem);

#endif
#endif
