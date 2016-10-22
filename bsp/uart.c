#include "bsp.h"

void uart_putc_poll(char c)
{
	while(*(UART0 + UARTFR) & UARTFR_TXFF);
	*UART0 = c;
}

