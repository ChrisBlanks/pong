/*
 *
 *
 * 
 */

#ifndef __GBA_DEFINES__
#define __GBA_DEFINES__

// device related defines

/// screen/display defines
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define VIRTUAL_SCREEN_WIDTH (SCREEN_WIDTH + 272)
#define VIRTUAL_SCREEN_HEIGHT (SCREEN_HEIGHT + 96)

/// memory map locations
#define MEM_IO   0x04000000
#define MEM_PAL  0x05000000
#define MEM_VRAM 0x06000000
#define MEM_OAM  0x07000000

#define REG_DISPLAY         (MEM_IO + 0x0000)
#define REG_DISPLAY_STATUS  (MEM_IO + 0x0004)
#define REG_DISPLAY_VCOUNT  (MEM_IO + 0x0006)
#define REG_KEY_INPUT       (MEM_IO + 0x0130)

//// reg display values
#define REG_DISPLAY_STATUS_VBLANK_INTERRUPT_ENABLE    0x0008
#define REG_DISPLAY_STATUS_HBLANK_INTERRUPT_ENABLE    0x0010
#define REG_DISPLAY_STATUS_V_COUNTER_INTERRUPT_ENABLE 0x0020

/// sound control registers
#define  SOUNDCNT_L 0x04000080
#define  SOUNDCNT_H 0x04000082
#define  SOUNDCNT_X 0x04000084
#define  SOUNDBIAS  0x04000088

/// sound channel A & B (Direct/DMA sound)
#define  SOUND_CHAN_FIFO_A_L 0x040000A0
#define  SOUND_CHAN_FIFO_A_H 0x040000A2

#define  SOUND_CHAN_FIFO_B_L 0x040000A4
#define  SOUND_CHAN_FIFO_B_H 0x040000A6

//// sound register values defines
#define SOUND_MASTER_ENABLE 0x80

/* bit patterns for the sound control register */
#define SOUND_A_RIGHT_CHANNEL 0x100
#define SOUND_A_LEFT_CHANNEL 0x200
#define SOUND_A_FIFO_RESET 0x800
#define SOUND_B_RIGHT_CHANNEL 0x1000
#define SOUND_B_LEFT_CHANNEL 0x2000
#define SOUND_B_FIFO_RESET 0x8000


/// DMA registers
////Only the 1st 27 bits matter

#define DMA0_SOURCE_ADDR_L   0x040000B0
#define DMA0_SOURCE_ADDR_H   0x040000B2 

#define DMA1_SOURCE_ADDR_L   0x040000BC
#define DMA1_SOURCE_ADDR_H   0x040000BE 

#define DMA2_SOURCE_ADDR_L   0x040000C8
#define DMA2_SOURCE_ADDR_H   0x040000CA 

#define DMA3_SOURCE_ADDR_L   0x040000D4
#define DMA3_SOURCE_ADDR_H   0x040000D6 


#define DMA0_DESTINATION_ADDR_L   0x040000B4
#define DMA0_DESTINATION_ADDR_H   0x040000B6 

#define DMA1_DESTINATION_ADDR_L   0x040000C0
#define DMA1_DESTINATION_ADDR_H   0x040000C2 

#define DMA2_DESTINATION_ADDR_L   0x040000CC
#define DMA2_DESTINATION_ADDR_H   0x040000CE 

#define DMA3_DESTINATION_ADDR_L   0x040000D8
#define DMA3_DESTINATION_ADDR_H   0x040000DA 


#define DMA0_WORD_COUNT   0x040000B8
#define DMA1_WORD_COUNT   0x040000C4 
#define DMA2_WORD_COUNT   0x040000D0
#define DMA3_WORD_COUNT   0x040000DC 

#define DMA0_CONTROL   0x040000BA
#define DMA1_CONTROL   0x040000C6 
#define DMA2_CONTROL   0x040000D2
#define DMA3_CONTROL   0x040000DE 

//// DMA register values defines
#define DMA_DEST_FIXED       0x00400000
#define DMA_REPEAT           0x02000000
#define DMA_SYNC_TO_TIMER_0  0x30000000

#define DMA_ENABLE 0x80000000
#define DMA_16 0x00000000
#define DMA_32 0x04000000


/// timers
#define REG_TM0_COUNT   0x04000100
#define REG_TM0_CONTROL 0x04000102

#define REG_TM1_COUNT   0x04000104
#define REG_TM1_CONTROL 0x04000106

#define REG_TM2_COUNT   0x04000108
#define REG_TM2_CONTROL 0x0400010A

#define REG_TM3_COUNT   0x0400010C
#define REG_TM3_CONTROL 0x0400010E

#define TIMER_1_CYCLE    0x0
#define TIMER_64_CYCLE   0x1
#define TIMER_256_CYCLE  0x2
#define TIMER_1024_CYCLE 0x3

#define TIMER_OVERFLOW_ENABLE  0x04
#define TIMER_INTERRUPT_ENABLE 0x40
#define TIMER_ENABLE           0x80

/// Key Input Masks (0 = pressed, 1 = released)
#define KEY_A         0x0001
#define KEY_B         0x0002
#define KEY_SELECT    0x0004
#define KEY_START     0x0008
#define KEY_RIGHT     0x0010
#define KEY_LEFT      0x0020
#define KEY_UP        0x0040
#define KEY_DOWN      0x0080
#define KEY_BUTTON_R  0x0100
#define KEY_BUTTON_L  0x0200

#define KEY_ANY    0x03FF

#define OBJECT_ATTR0_Y_MASK 0x0FF
#define OBJECT_ATTR1_X_MASK 0x1FF

/// interrupts

//// registers
#define INTERRUPT_MASTER_ENABLE       0x04000208
#define INTERRUPT_INDIVIDUAL_ENABLE   0x04000200
#define INTERRUPT_REQUEST_STATUS      0x04000202

#define INTERRUPT_JUMP_ADDRESS        0x03007FFC
#define INTERRUPT_CHECK_FLAG          0x03007FF8

//// interrupt values
#define INTERRUPT_VBLANK          0x0001
#define INTERRUPT_HBLANK          0x0002
#define INTERRUPT_V_COUNTER_MATCH 0x0004
#define INTERRUPT_TIMER_0         0x0008
#define INTERRUPT_TIMER_1         0x0010
#define INTERRUPT_TIMER_2         0x0020
#define INTERRUPT_TIMER_3         0x0040
#define INTERRUPT_COMMS           0x0080
#define INTERRUPT_DMA0            0x0100
#define INTERRUPT_DMA1            0x0200
#define INTERRUPT_DMA2            0x0400
#define INTERRUPT_DMA3            0x0800
#define INTERRUPT_KEY             0x1000
#define INTERRUPT_GAME_PAK        0x2000


/// typedefs
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef uint16 rgb15;

//128 objects can exist in OAM & each one needs a set of attributes
// Object attributes consist of 48 bits w/ a width of 16 bits per memory address
// Directly following the 48 bits is a 16 bit section for rotation/scaling parameters
typedef struct obj_attrs {
    uint16 attr0; //controls y-coordinate (lower 8 bits), shape, and colour mode of object tiles (4bpp or 8bpp)
                  /* format (high to low): 
                    | OBJ shape | color mode | obj. mosaic toggle | obj. mode | rotation/scaling double flag | rotation/scaling flag| y-coordinate |
                        2-bit       1-bit             1-bit               2-bit               1-bit                   1-bit               8-bit
                  */
    uint16 attr1; //controls x-coordinate (lower 9 bits) and the size of the object
                  /* format (high to low): 
                    | OBJ size | vertical flip flag | horizontal flip flag | rotation/scaling parameter | x-coordinate | 
                        2-bit       1-bit                   1-bit                       5-bit                8-bit
                    *** Note *** : 
                  */

    uint16 attr2; //controls base tile index of object and colour palette the object should use (when in 4bpp mode)
    uint16 pad;
} __attribute__((packed, aligned(4))) obj_attrs;

typedef uint32 tile_4bpp[8]; //create a uint32 array of 8 elements
typedef tile_4bpp tile_block[512]; //create a uint32 2D array 

#define OAM_MEM             ((volatile obj_attrs*) MEM_OAM)
#define TILE_MEM            ((volatile tile_block*) MEM_VRAM)
#define OBJECT_PALETTE_MEM  ((volatile rgb15*) (MEM_PAL + 0x200))


// miscellaneous defines
#define CLOCK_HZ 16777216 
#define CYCLES_PER_BLANK 280806


//global variables
#ifdef __GBA_VARIABLES_MAIN__

//initialize variables if symbol is defined in the main source code file for the variables
unsigned int CHANNEL_A_VBLANKS_REMAINING = 0; //counts remaining VBlanks before sound playback is finished
unsigned int CHANNEL_A_VBLANKS_TOTAL = 0; //number of required VBlanks for sound to play

#else

//only declare the variables if they are needed to be referenced in other files 
extern unsigned int CHANNEL_A_VBLANKS_REMAINING; //counts remaining VBlanks before sound playback is finished
extern unsigned int CHANNEL_A_VBLANKS_TOTAL; //number of required VBlanks for sound to play

#endif 


#endif // ifndef __GBA_DEFINES__