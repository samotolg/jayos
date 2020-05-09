SOURCE_TOP := $(shell pwd)
include $(SOURCE_TOP)/Makefile.config

TARGET_NAME := fos
TARGET	:= $(SOURCE_TOP)/$(TARGET_NAME).bin
ELFS	:= $(SOURCE_TOP)/$(TARGET_NAME).elf
LINKER	:= $(SOURCE_TOP)/$(TARGET_NAME).ld
MAP	:= $(SOURCE_TOP)/$(TARGET_NAME).map

#ASFLAGS := -g -mthumb-interwork -mcpu=cortex-r4 
ASFLAGS := -g -mthumb-interwork -march=armv6 -marm
ASFLAGS += -I$(SOURCE_TOP) -I$(SOURCE_TOP)/include -I$(SOURCE_TOP)/os -I$(SOURCE_TOP)/usr -I$(SOURCE_TOP)/bsp
#CCFLAGS := -Os -g -mno-thumb-interwork -mcpu=cortex-r4 -Wall -Werror
CCFLAGS := -Os -g -mno-thumb-interwork -mcpu=ARM926EJ-S -Wall
CCFLAGS += -ffreestanding -nostdlib -fno-builtin -static
CCFLAGS += -I$(SOURCE_TOP) -I$(SOURCE_TOP)/include -I$(SOURCE_TOP)/os -I$(SOURCE_TOP)/usr -I$(SOURCE_TOP)/bsp

#CXFLAGS	:= -O0 -g -mno-thumb-interwork -march=armv6 -marm -Wall -Werror
CXFLAGS	:= -O0 -g -mno-thumb-interwork -mcpu=cortex-r4 -Wall -Werror
CXFLAGS += -ffreestanding -nostdlib -fno-builtin -static

LDFLAGS := -static -nostdlib -Map=$(MAP) -N -T $(LINKER)

OBJCOPYFLAGS := -x -S --output-target binary 

INCS	:= -I$(SOURCE_TOP)/include \
	-I$(SOURCE_TOP)/bsp

OBJS	:= bsp/bootstrap.o	\
	bsp/exception_handlers.o \
	bsp/aeabi.o				\
	bsp/gic.o				\
	bsp/uart.o				\
	bsp/vsprintf.o			\
	bsp/debug.o				\
	usr/main.o				\
	usr/task.o				\
	os/fos.o				\
	os/fosarm.o 			\
	os/fos_timer.o			\
	os/os_sem.o				\
	os/os_mutex.o

OBJS := $(patsubst %.o, $(SOURCE_TOP)/%.o, $(OBJS))

.PHONY: all clean rebuild

all: $(TARGET)

run: $(TARGET)
	qemu-system-arm -M versatilepb -kernel fos.elf -nographic

rebuild: clean all

clean:
	@rm -f $(OBJS) $(OBJS_LIB) $(TARGET) $(ELFS) $(MAP)

$(TARGET): $(ELFS)
	$(OCP_BIN)

$(ELFS): $(OBJS) $(OBJS_LIB)
	$(LD_ELF)

$(SOURCE_TOP)/%.o: $(SOURCE_TOP)/%.c
	$(GCC_OBJ)

$(SOURCE_TOP)/%.o: $(SOURCE_TOP)/%.S
	$(ASM_OBJ)

