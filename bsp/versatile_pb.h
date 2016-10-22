#ifndef VERSATILE_PB_H
#define VERSATILE_PB_H

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

#define UART_OFFSET				0x10000

#define	DEBUG_UART_CH			1

#define TX_FIFO_FULL			(1 << 24)

#define UFSTATn(ch)				(UART_BASE + (ch * UART_OFFSET) + 0x18)
#define UTXHn(ch)				(UART_BASE + (ch * UART_OFFSET) + 0x20)
#define URXHn(ch)				(UART_BASE + (ch * UART_OFFSET) + 0x24)
#define UBRDIVn(ch)				(UART_BASE + (ch * UART_OFFSET) + 0x28)
#define UFRACVALn(ch)			(UART_BASE + (ch * UART_OFFSET) + 0x2C)

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/Bbabegge.html */
#define UART0					((volatile char *)0x101f1000)

#define UARTFR 					0x06
#define UARTFR_TXFF 			0x20

#endif
