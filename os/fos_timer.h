#ifndef OS_TIMER_H
#define OS_TIMER_H

#include "bsp.h"
#include "artik_board_config.h"
#include "irqs.h"
#include "gic.h"

#if 0
#define MCT_CH_OFFSET		0x100

#define MCT_L_TCNTCB(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x00)
#define MCT_L_TCNTCO(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x04)
#define MCT_L_TCON(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x20)
#define		L_TCON_FRC_TIMER	 											0x08
#define		L_TCON_INT_TYPE_ONESHOT											0x04
#define		L_TCON_INT_START												0x02
#define		L_TCON_TIMER_													0x01
#define MCT_L_INT_CSTAT(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x30)
#define MCT_L_INT_ENB(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x34)
#define MCT_L_WSTAT(ch)		(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x40)

#define MCT_L_FRCNTB(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x10)
#define MCT_L_FRCNTO(ch)	(MCT_L_BASE + (MCT_CH_OFFSET * ch) + 0x14)

#define MCT_COMP_OFFSET		0x10
#define MCT_G_CNTL			(MCT_G_BASE)
#define MCT_G_CNTU			(MCT_G_BASE + 0x004)
#define MCT_G_CNT_WSTAT		(MCT_G_BASE + 0x010)
#define MCT_G_COMP_L(ch)	(MCT_G_BASE + 0x100 + MCT_COMP_OFFSET*ch + 0x00)
#define MCT_G_COMP_U(ch)	(MCT_G_BASE + 0x100 + MCT_COMP_OFFSET*ch + 0x04)
#define MCT_G_AUTO_INC(ch)	(MCT_G_BASE + 0x100 + MCT_COMP_OFFSET*ch + 0x08)
#define MCT_G_TCON			(MCT_G_BASE + 0x140)
#define 	MCT_G_TCON_ENABLE			0x100
#define		MCT_G_TCON_AUTO_INC(ch)		(0x2 << (ch*2))
#define		MCT_G_TCON_COMP_EN(ch)		(0x1 << (ch*2))
#define	MCT_G_INT_CSTAT		(MCT_G_BASE + 0x144)
#define	MCT_G_INT_ENB		(MCT_G_BASE + 0x148)
#define	MCT_G_WSTAT			(MCT_G_BASE + 0x14C)

#else

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/index.html */
//#define TIMER_OFFSET 			((volatile unsigned int*)0x101E2000
#define TIMER_OFFSET 			0x101E2000
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
#endif


#define TIMER_1MS				1000
#define TIMER_10MS				10000
#define TIMER_100MS				100000
#define TIMER_1S				1000000
#define TIMER_10S				10000000

#define OS_ROUND_ROBIN_CHANNEL								0
#define OS_TIMER_CHANNEL									1


typedef void (*timer_cb_func_type)( int );

timer_cb_func_type 	os_timer_cb[2];

#endif
