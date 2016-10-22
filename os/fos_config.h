#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#define OS_SCHEDULE_ROUND_ROBIN
#define OS_SEMAPHORE
#define OS_MUTEX
#define OS_PRIORITY_INHERITANCE


/* If the following is defined, debug messages will come out thru serial port */
#if 0
#define OS_DEBUG_SIG
#define OS_DEBUG_TIMER
#define OS_DEBUG_SEMAPHORE
#define OS_DEBUG_MUTEX
#define OS_PRIORITY_INHERITANCE
#endif

#endif
