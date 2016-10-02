CC = /usr/local/arm-2008q3/bin/arm-none-eabi-gcc
LD = /usr/local/arm-2008q3/bin/arm-none-eabi-ld

CFLAGS = -march=armv6 

LFLAGS = -N -Ttext=0x10000

all:main.elf

main.elf:main.o

.o.c:
	$(CC) -c $(CFLAGS) -o $@ $^

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LFLAGS)	-o $@ $^
	
clean:
	rm *.o
	rm main.elf	
