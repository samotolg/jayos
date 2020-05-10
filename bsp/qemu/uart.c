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

int uart_putc_poll(char c)
{
	while(*(UART0 + UARTFR) & UARTFR_TXFF);
	*UART0 = c;
}

