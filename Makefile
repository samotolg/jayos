CC = /usr/local/arm-2008q3/bin/arm-none-eabi-gcc
LD = /usr/local/arm-2008q3/bin/arm-none-eabi-ld

#CFLAGS = -ansi -pedantic -Wall -Wextra -march=armv6 -msoft-float -fPIC -mapcs-frame
CFLAGS = -march=armv6 

LFLAGS = -N -Ttext=0x10000

all:main.elf

.o.c:
	$(CC) -c $(CFLAGS) -o $@ $^

.SUFFIXES: .o .elf
.o.elf:main.o
	$(LD) $(LFLAGS)	-o $@ $^
	
clean:
	rm *.o
	rm main.elf	
