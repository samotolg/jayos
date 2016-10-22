#ifndef HW_TIMER_H
#define HW_TIMER_H

#include "bsp.h"

typedef void (*timer_cb_func_type)( int );

timer_cb_func_type 	os_timer_cb[2];


void os_hw_timer_init();
void os_hw_timer_start();
u32 os_hw_timer_get_count(u8 ch);
void os_hw_timer_enable(u8 ch, u32 time, timer_cb_func_type func);
void os_hw_timer_disable(u8 ch); 
void os_hw_timer_pause(u8 ch);
void os_hw_timer_resume(u8 ch);

#endif
