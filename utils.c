#include "versatilepb.h"

void print(char *s) 
{
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s++;
	}
}
