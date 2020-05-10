#include "stdint.h"
#include "uart.h"
#include "hal_uart.h"
#include "hal_interrupt.h"

extern volatile PL011_t* Uart;

static void interrupt_handler(void);

void hal_uart_init(void)
{
	debug_printf("[uart.c] hal_uart_init\n");

	// Enable UART
	Uart->uartcr.bits.UARTEN = 0;
	Uart->uartcr.bits.TXE = 1;
	Uart->uartcr.bits.RXE = 1;
	Uart->uartcr.bits.UARTEN = 1;

	// Enable input interrupt
	Uart->uartimsc.bits.RXIM = 1;

	// Register UART interrupt handler
	hal_interrupt_enable(UART_INTERRUPT);
	hal_interrupt_register_handler(interrupt_handler, UART_INTERRUPT);
}

void hal_uart_put_char(uint8_t ch)
{
	while(Uart->uartfr.bits.TXFF);	// waiting output buffer to 0
	Uart->uartdr.all = (ch & 0xFF);	// send ch to output buffer
}

uint8_t hal_uart_get_char(void)
{
	uint32_t data;

	while(Uart->uartfr.bits.RXFE);

	data = Uart->uartdr.all;

	// check for an error flag
	if (data & 0xFFFFFF00)
	{
		// clear the error
		Uart->uartrsr.all = 0xFF;
		return 0;
	}

	return (uint8_t)(data & 0xFF);
}

static void interrupt_handler(void)
{
	uint8_t ch = hal_uart_get_char();
	hal_uart_put_char(ch);
}

