#include <stdarg.h>

extern void uart_putc_poll(char c);
extern int d_vsprintf(char *buf, const char *fmt, va_list args);


static void uart_putstring(char *str, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		uart_putc_poll(str[i]);
	}
}

void printf(const char *fmt,...)
{
	char buf[80];
	va_list ap;
	int size = 0;

	va_start(ap, fmt);
	size = d_vsprintf(buf, (const char *)fmt, ap);
	if (size > sizeof(buf)) {
		return;
	}
	va_end(ap);

	uart_putstring(buf, size);
}


