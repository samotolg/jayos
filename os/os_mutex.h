#ifndef OS_MUTEX_H
#define OS_MUTEX_H
#ifdef OS_MUTEX

#include "fos.h"

extern void os_mutex_init(os_mutex_type *mutex);
extern int os_mutex_wait(os_mutex_type *mutex);
extern int os_mutex_post(os_mutex_type *mutex);

#endif
#endif // OS_MUTEX_H

