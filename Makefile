CC = /usr/local/arm-2008q3/bin/arm-none-eabi-gcc
LD = /usr/local/arm-2008q3/bin/arm-none-eabi-ld

CFLAGS = -march=armv6 

LFLAGS = -N -Ttext=0x10000

all:main.elf

main.elf:main.o

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $^

.SUFFIXES: .o .elf
.o.elf:main.o
	$(LD) $(LFLAGS)	-o $@ $^
	
clean:
	rm *.o
	rm main.elf	
