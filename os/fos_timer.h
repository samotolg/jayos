#ifndef OS_TIMER_H
#define OS_TIMER_H

#include "bsp.h"
#include "rtos_type.h"
#include "hw_timer.h"

#define TIMER_1MS				1000
#define TIMER_10MS				10000
#define TIMER_100MS				100000
#define TIMER_1S				1000000
#define TIMER_10S				10000000

#define OS_ROUND_ROBIN_CHANNEL								0
#define OS_TIMER_CHANNEL									1


void os_timer_init();
void os_timer_start();
u32 os_timer_get_count(u8 ch);
void os_timer_enable(u8 ch, u32 time, timer_cb_func_type func);
void os_timer_disable(u8 ch);
void os_timer_pause(u8 ch);
void os_timer_resume(u8 ch);
u32 os_timer_get_global_count();

#endif
