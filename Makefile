#Note: Requires devkitarm executables to be discoverable in the system path

#define compiler and flags
CC := arm-none-eabi-gcc
CFLAGS := -mthumb-interwork -mthumb -O2
ARMFLAGS := -mthumb-interwork -marm -O2

#define other build steps
ELFCOPY := arm-none-eabi-objcopy
GBAFIX := gbafix 

#define relevant directories
TARGET := pong
BUILD  := build
ODIR    := obj
SOURCES := src
INCLUDES := include

_DEPS := include/gba_defines.h  include/gba_graphics.h  include/gba_helper_funcs.h  include/test.h 
_IWRAM_DEPS := include/vblanks_interrupt.iwram.h
_SRC := src/main.c src/gba_graphics.c src/gba_helper_funcs.c
_IWRAM_SRC := src/vblanks_interrupt.iwram.c
_OBJS := build/main.o build/gba_graphics.o build/gba_helper_funcs.o build/vblanks_interrupt.iwram.o

ARCH := 
LIBS := 

GBASPECS := C:\devkitPro\devkitARM\arm-none-eabi\lib\gba.specs

#To-Do: Make rule that executes all rules. Add rule that moves object files into the correct folder
.PHONY: all
all: buildthumb buildarm moveobjects buildelf buildgba gbafix

#define rule to make object files dependent on .c and .h files
buildthumb  : $(_SRC) $(_DEPS)
	$(CC) -c $(_SRC) $(CFLAGS) -I$(INCLUDES)

buildarm  : $(_IWRAM_SRC) $(_IWRAM_DEPS)
	$(CC) -c $(_IWRAM_SRC) $(ARMFLAGS) -I$(INCLUDES)

moveobjects :
	mkdir $(BUILD); mv *.o $(BUILD)/

#link object files into elf file
buildelf : $(_OBJS)
	$(CC) $(_OBJS) -mthumb-interwork -mthumb -specs=$(GBASPECS) -o $(BUILD)/$(TARGET).elf

#create a gba file from the elf file
buildgba : $(BUILD)/$(TARGET).elf
	$(ELFCOPY) -v -O binary $< $(BUILD)/$(TARGET).gba

#fix gba header
gbafix : $(BUILD)/$(TARGET).gba
	$(GBAFIX) $<


.PHONY: clean
clean:
	rm -f $(BUILD)/*.o *.o