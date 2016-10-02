#define UART0		((volatile char *)0x101f1000)

int main(void)
{
	char *string = "Hello, World!\n";
	while (*string)
		*UART0 = *string++;
		
	while(1);	
	
	return 0;
}
