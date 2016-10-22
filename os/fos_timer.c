#include "fos_timer.h"
#include "hw_timer.h"


void os_timer_init()
{
	os_hw_timer_init();
}

void os_timer_start()
{
	os_hw_timer_start();
}

u32 os_timer_get_count(u8 ch)
{
	return os_hw_timer_get_count(ch);
}

void os_timer_enable(u8 ch, u32 time, timer_cb_func_type func)
{
	os_hw_timer_enable(ch, time, func);
}

void os_timer_disable(u8 ch)
{
	os_hw_timer_disable(ch);
}

void os_timer_pause(u8 ch)
{
	os_hw_timer_pause(ch);
}

void os_timer_resume(u8 ch)
{
	os_hw_timer_resume(ch);
}

u32 os_timer_get_global_count()
{
	return 1;
}