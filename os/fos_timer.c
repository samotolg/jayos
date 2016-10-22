#include "fos_timer.h"



void os_hw_timer_init()
{
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
}

u32 os_hw_timer_get_count(u8 ch)
{
	return *(volatile unsigned int *)(TIMER_VALUE(ch));
}

void os_hw_timer_enable(u8 ch, u32 time, timer_cb_func_type func)
{
	*(volatile unsigned int *)(TIMER_LOAD(ch)) = time;
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
	os_timer_cb[ch] = func;
}

void os_hw_timer_disable(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_ONESHOT | TIMER_32BIT;
}

void os_hw_timer_pause(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_ONESHOT | TIMER_32BIT;
}

void os_hw_timer_resume(u8 ch)
{
	*(volatile unsigned int *)(TIMER_CONTROL(ch)) = TIMER_EN | TIMER_ONESHOT | TIMER_32BIT | TIMER_INTEN;
}

u32 os_hw_timer_get_global_count()
{
	return 1;
}