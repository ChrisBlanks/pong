#Note: Requires devkitarm executables to be discoverable in the system path

#define compiler and flags
CC := arm-none-eabi-gcc
CFLAGS := -mthumb-interwork -mthumb -O2 -Wall -g
ARMFLAGS := -mthumb-interwork -marm -O2 -Wall -g

#define other build steps
ELFCOPY := arm-none-eabi-objcopy
GBAFIX := gbafix 

#define project directories
TARGET := pong
BUILD  := build
ODIR    := obj
SOURCES := src
INCLUDES := include

DEPS := $(INCLUDES)/gba_defines.h  $(INCLUDES)/gba_graphics.h  $(INCLUDES)/gba_helper_funcs.h  $(INCLUDES)/test.h $(INCLUDES)/pong.h
SRC := $(SOURCES)/main.c $(SOURCES)/gba_graphics.c $(SOURCES)/gba_helper_funcs.c $(SOURCES)/pong.c
IWRAM_DEPS := $(INCLUDES)/vblanks_interrupt.iwram.h
IWRAM_SRC := $(SOURCES)/vblanks_interrupt.iwram.c

OBJS := $(BUILD)/main.o $(BUILD)/gba_graphics.o $(BUILD)/gba_helper_funcs.o $(BUILD)/vblanks_interrupt.iwram.o $(BUILD)/pong.o

ARCH := 
LIBS := 

LIBGBA := C:\libgba

GBASPECS := C:\devkitPro\devkitARM\arm-none-eabi\lib\gba.specs

#To-Do: Make rule that executes all rules. Add rule that moves object files into the correct folder
.PHONY: all clean cleanall
all: buildthumb buildarm moveobjects buildelf buildgba gbafix

clean:
	rm -f $(BUILD)/*.o *.o

cleanall:
	rm -r $(BUILD)/

### actual targets

#define rule to make object files dependent on .c and .h files
buildthumb  : $(SRC) $(DEPS)
	$(CC) -c $(SRC) $(CFLAGS) -I$(INCLUDES) -I$(LIBGBA)\$(INCLUDES)

buildarm  : $(IWRAM_SRC) $(IWRAM_DEPS)
	$(CC) -c $(IWRAM_SRC) $(ARMFLAGS) -I$(INCLUDES) 

moveobjects :
	mkdir $(BUILD); mv *.o $(BUILD)/

#link object files into elf file
buildelf : $(_OBJS)
	$(CC) $(OBJS) -LC:\libgba\lib  -lgba -mthumb-interwork -mthumb -specs=$(GBASPECS) -o $(BUILD)/$(TARGET).elf
# alternative:	$(CC) $(OBJS) C:\libgba\lib\libgba.a -mthumb-interwork -mthumb -specs=$(GBASPECS) -o $(BUILD)/$(TARGET).elf  

#create a gba file from the elf file
buildgba : $(BUILD)/$(TARGET).elf
	$(ELFCOPY) -v -O binary $< $(BUILD)/$(TARGET).gba

#fix gba header
gbafix : $(BUILD)/$(TARGET).gba
	$(GBAFIX) $<
