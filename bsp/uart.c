#include "reg_macro.h"
#include "artik_board_config.h"

#define UART_OFFSET	0x10000

#define	DEBUG_UART_CH	1

#define TX_FIFO_FULL	(1 << 24)

#define UFSTATn(ch)		(UART_BASE + (ch * UART_OFFSET) + 0x18)
#define UTXHn(ch)		(UART_BASE + (ch * UART_OFFSET) + 0x20)
#define URXHn(ch)		(UART_BASE + (ch * UART_OFFSET) + 0x24)
#define UBRDIVn(ch)		(UART_BASE + (ch * UART_OFFSET) + 0x28)
#define UFRACVALn(ch)	(UART_BASE + (ch * UART_OFFSET) + 0x2C)

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/Bbabegge.html */
#define UART0			((volatile char *)0x101f1000)

#define UARTFR 			0x06
#define UARTFR_TXFF 	0x20


void uart_init(void)
{
	writel(0x11220000, GPP0_CON);

	// baud rate setting 115200
	writel(0x0d, UBRDIVn(DEBUG_UART_CH));
	writel(0x0b, UFRACVALn(DEBUG_UART_CH));
}

char uart_getc_poll(void)
{
	unsigned long ch; 
	while(!(readl(UFSTATn(DEBUG_UART_CH)) & 0xff));
	ch = URXHn(DEBUG_UART_CH);

	return (char)ch; 
}

int uart_putc_poll(char c)
{
#if 0
	if(c == '\n') {
		while (readl(UFSTATn(DEBUG_UART_CH)) & TX_FIFO_FULL);
		writel('\r', UTXHn(DEBUG_UART_CH));
	}

	while (readl(UFSTATn(DEBUG_UART_CH)) & TX_FIFO_FULL);
	writel(c, UTXHn(DEBUG_UART_CH));
	return c;
#else
	while(*(UART0 + UARTFR) & UARTFR_TXFF);
	*UART0 = c;
#endif		
}

