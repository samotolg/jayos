#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include "fos.h"

#ifdef OS_MUTEX

void os_mutex_init(os_mutex_type *mutex);
int os_mutex_wait(os_mutex_type *mutex);
int os_mutex_post(os_mutex_type *mutex);

#endif
#endif // OS_MUTEX_H

