CC = /usr/local/arm-2008q3/bin/arm-none-eabi-gcc
LD = /usr/local/arm-2008q3/bin/arm-none-eabi-ld

CFLAGS = -Wall -Wextra -march=armv6 -fPIC -marm

FLAGS = -N -Ttext=0x10000

all:main.elf

main.elf:bootstrap.o exception_handlers.o main.o utils.o

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $^

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LFLAGS)	-o $@ $^
	
.SUFFIXES: .s .o
.s.o:
	$(CC) -c $(CFLAGS) -o $@ $^	
	
clean:
	rm *.o
	rm main.elf	
