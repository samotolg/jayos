CC = /usr/local/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-gcc
LD = /usr/local/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-ld

CFLAGS = -Wall -Wextra -march=armv6 -fPIC -marm
LFLAGS = -N -Ttext=0x10000

all:main.elf

main.elf:bootstrap.o exception_handlers.o main.o utils.o fos.o fosarm.o

.SUFFIXES: .c .o
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
