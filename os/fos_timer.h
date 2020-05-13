#ifndef OS_TIMER_H
#define OS_TIMER_H

#include "bsp.h"

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/index.html */
//#define TIMER_OFFSET 			((volatile unsigned int*)0x101E2000

#define TIMER_OFFSET 			0x1c110000
#define TIMER_GAP				0x20

#define TIMER_LOAD(ch)			(volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0x0)
#define TIMER_VALUE(ch)			(volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0x4) 	
#define TIMER_CONTROL(ch) 		(volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0x8) 	
#define TIMER_INTCLR(ch)		(volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0xC) 	
#define TIMER_RIS(ch)			(volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0x10)	
#define TIMER_MIS(ch) 			((volatile unsigned int *)(TIMER_OFFSET + (TIMER_GAP*ch) + 0x14))

/* TIMER_CONTROL bit assignment */
#define TIMER_EN 				0x80
#define TIMER_PERIODIC 			0x40
#define TIMER_INTEN 			0x20
#define TIMER_32BIT 			0x02
#define TIMER_ONESHOT 			0x01

/* http://infocenter.arm.com/help/topic/com.arm.doc.dui0224i/I1042232.html */
#define PIC ((volatile unsigned int*)0x10140000)		/* Primary Interrupt Controller IRQ status register */
#define PIC_TIMER01				0x10					/* RTC */
/* http://infocenter.arm.com/help/topic/com.arm.doc.ddi0181e/I1006461.html */
#define VIC_INTENABLE 			0x4 /* 0x10 bytes */

#define TIMER_1MS				1000
#define TIMER_10MS				10000
#define TIMER_100MS				100000
#define TIMER_1S				1000000
#define TIMER_10S				10000000

#define OS_ROUND_ROBIN_CHANNEL								0
#define OS_TIMER_CHANNEL									1

#define TIMER_INTERRUPT          34


typedef void (*timer_cb_func_type)( int );

timer_cb_func_type 	os_timer_cb[2];

#endif
