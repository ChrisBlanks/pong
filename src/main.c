/*
 * Author: ChrisB
 * Purpose: Pong game
 */
 
#define __GBA_VARIABLES_MAIN__
#include "gba_defines.h"

//project includes
#include "gba_graphics.h"
#include "gba_helper_funcs.h"
#include "test.h"

#include "vblanks_interrupt.iwram.h"

//devkitarm C lib includes
#include <stdio.h>
//#include <stdlib.h>

//libgba includes
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>


//sample rate of test audio sample
#define TEST_AUDIO_SAMPLE_RATE 44100


//global variables

/// define the timer data and control registers 
volatile unsigned short* timer0_data = (volatile unsigned short*) REG_TM0_COUNT;
volatile unsigned short* timer0_control = (volatile unsigned short*) REG_TM0_CONTROL;

volatile unsigned short* timer1_data = (volatile unsigned short*) REG_TM1_COUNT;
volatile unsigned short* timer1_control = (volatile unsigned short*) REG_TM1_CONTROL;

volatile unsigned short* timer2_data = (volatile unsigned short*) REG_TM2_COUNT;
volatile unsigned short* timer2_control = (volatile unsigned short*) REG_TM2_CONTROL;

//define fifo buffers for sound
volatile unsigned char* fifo_buffer_a = (volatile unsigned char*)SOUND_CHAN_FIFO_A_L;
volatile unsigned char* fifo_buffer_b = (volatile unsigned char*)SOUND_CHAN_FIFO_B_L;

//define sound registers
volatile unsigned short* sound_reg = (volatile unsigned short* )SOUNDCNT_H;
volatile unsigned short* master_sound_reg = (volatile unsigned short*)SOUNDCNT_X;

//define dma registers
volatile unsigned int* dma_1_source = (volatile unsigned int*) DMA1_SOURCE_ADDR_L;
volatile unsigned int* dma_1_destination = (volatile unsigned int*) DMA1_DESTINATION_ADDR_L;
volatile unsigned int* dma_1_control = (volatile unsigned int*) DMA1_WORD_COUNT;

//setup interrupts
volatile unsigned short* interrupt_master = (volatile unsigned short*)INTERRUPT_MASTER_ENABLE;
volatile unsigned short* interrupt_individual = (volatile unsigned short*)INTERRUPT_INDIVIDUAL_ENABLE;
volatile unsigned short* interrupt_status = (volatile unsigned short*)INTERRUPT_REQUEST_STATUS;
volatile unsigned int* interrupt_callback_addr = (volatile unsigned int*)INTERRUPT_JUMP_ADDRESS;
volatile unsigned short* interrupt_check_flag = (volatile unsigned short*)INTERRUPT_CHECK_FLAG;

volatile unsigned short* display_reg_status = (volatile unsigned short*)REG_DISPLAY_STATUS;

volatile unsigned short* reg_key_input = (volatile unsigned short*) REG_KEY_INPUT;
volatile unsigned int* reg_display = (volatile unsigned int*) REG_DISPLAY;



///helper functions

// Form a 16-bit RGB GBA color from three component values
static inline rgb15 RGB15(int red, int green, int blue){
    return red | (green << 5) | (blue << 10) ;
}

// Set the position of an object to specified X and Y coordinates
static inline void setObjectPosition(volatile obj_attrs* object, int x_pos, int y_pos){
    object->attr0 = (object->attr0 & ~OBJECT_ATTR0_Y_MASK) | (y_pos & OBJECT_ATTR0_Y_MASK);
    object->attr1 = (object->attr1 & ~OBJECT_ATTR1_X_MASK) | (x_pos & OBJECT_ATTR1_X_MASK);
}

// Clamp 'value' in the range of 'min' and 'max'
static inline int clampValue(int value, int min, int max){
    return (value < min) ? min : ( (value > max) ? max : value) ;
}

void vblankInterruptWait(){
    asm volatile("swi 0x05"); //0x05 is VblankIntrWait in the GBA function table
}

void setupOneSecTimer(){
    //disable timers before making changes
    *timer1_control = 0x0;
    *timer2_control = 0x0;

    //setup timer 1
    *timer1_data = 0x0;
    *timer1_control = TIMER_256_CYCLE| TIMER_ENABLE; //set timer 1 to 1024 cycles per increment (16.384 kHz, 61.04 micro seconds)

    //setup timer 2 to allow overflows from timer 1
    *timer2_control = TIMER_OVERFLOW_ENABLE | TIMER_ENABLE; 
}

//sound functions
void setupSoundTimers(){
    *timer0_control = 0x0; //disable timer 0

    unsigned short ticks_per_sample = CLOCK_HZ / TEST_AUDIO_SAMPLE_RATE; //divide clock speed (~16.78 MHz) by audio sample rate

    //set timer to max number of ticks before overflow minus num of ticks per sample, so that timer overflows after number of ticks for a single sample
    *timer0_data = 65536 - ticks_per_sample; 

    //divide total number of ticks (fraction of clock time) by number of cycles for a single V blank period
    CHANNEL_A_VBLANKS_REMAINING = test_bytes * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
    CHANNEL_A_VBLANKS_TOTAL = CHANNEL_A_VBLANKS_REMAINING;

    *timer0_control = TIMER_ENABLE | TIMER_1_CYCLE; //enable timer 0 for 1 cycle mode (16.78 MHz frequency)
}


void setupSoundTest(){
    //disable timer and dma before changes
    *timer0_control = 0x0;
    *dma_1_control = 0x0;

    //configure direct sound control register for sound A
    *sound_reg |= SOUND_A_RIGHT_CHANNEL |  SOUND_A_LEFT_CHANNEL | SOUND_A_FIFO_RESET;

    //enable all sounds 
    *master_sound_reg = SOUND_MASTER_ENABLE; //enable all sounds

    //set dma source address for test data address
    *dma_1_source = (unsigned int)  test_sound_data; //set dma source value to address of test sound data

    //setup dma destination address fifo a
    *dma_1_destination = (unsigned int)  fifo_buffer_a; //set dma destination to fifo a

    //setup dma 1 for sending test data
    //timer 0 overflows load next sound data into buffer via DMA
    *dma_1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER_0 | DMA_ENABLE;
}

void setupInitialInterrupts(){
    *interrupt_master = 0; //disable master interrupt

    *interrupt_callback_addr = (unsigned int) &executeVBlankInterrupt; //set jump address to callback func memory location
    *display_reg_status |= REG_DISPLAY_STATUS_VBLANK_INTERRUPT_ENABLE; //enable vblank interrupt in register
    *interrupt_individual |= INTERRUPT_VBLANK; //enable VBLANK interrupt
    
    *interrupt_master = 0x1; //enable master interrupt

    *sound_reg = 0x0; //clear sound control 
}


int main(void){

    setupInitialInterrupts();
    setupSoundTest();
    setupSoundTimers();
    setupOneSecTimer();
    
    //consoleDemoInit();
    consoleInit(    0,		// charbase
					4,		// mapbase
					0,		// background number
					NULL,	// font
					0, 		// font size
					15		// 16 color palette
                );

    //BG_COLORS[0]=RGB8(58,110,165);
    BG_COLORS[0]=RGB8(0,127,0);
	BG_COLORS[241]=RGB5(31,31,31);

    //select BG mode 0, select BG0/BG1/OBJ, and set OBJ character to be handled in memory 1-dimensional
    SetMode(MODE_0 | BG0_ON | BG1_ON | OBJ_ON | OBJ_1D_MAP);
    //To-Do: Fix issue w/ weird background

    // Write the tiles for our sprites into the fourth tile block in VRAM
    // Four tiles for an 8x32 paddle sprite, and one tile for an 8x8 ball sprite
    // Using 4bpp, 0x1111 is four pixels of color index 1. 0x2222 is four pixels of color index 2
    volatile uint16* paddleTileMem= (uint16*)TILE_MEM[4][1];
    volatile uint16* ballTileMem= (uint16*)TILE_MEM[4][5];

    for(int i=0; i < 4 *(sizeof(tile_4bpp)/2); ++i){
        paddleTileMem[i] = 0x1111;
    }

    for(int i=0; i < (sizeof(tile_4bpp)/2); ++i){
        ballTileMem[i] = 0x2222;
    }

    // Write the color palette for our sprites into the 1st palette of 16 colors in color palette memory
    OBJECT_PALETTE_MEM[1] = RGB15(0x1F,0x1F,0x1F); // white color
    OBJECT_PALETTE_MEM[2] = RGB15(0x1F,0x00,0x1F); // magenta color
    
    //create the sprites by writing their object attributes into OAM memory
    volatile obj_attrs* paddleAttrs = &OAM_MEM[0];
    paddleAttrs->attr0 = 0x8000; // 4bpp tiles, TALL shape
    paddleAttrs->attr1 = 0x4000; // 8x32 size when using the TALL shape
    paddleAttrs->attr2 = 1; // start at the 1st tile in tile block 4 & use color palette zero

    volatile obj_attrs* ballAttrs = &OAM_MEM[1];
    ballAttrs->attr0 = 0; // 4bpp tiles, SQUARE shape
    ballAttrs->attr1 = 0; // 8x8 size when using the SQUARE shape
    ballAttrs->attr2 = 5; // start at the 5th tile in tile block four & use color palette zero

    //initialize variables to keep track of the state of the paddle & ball
    //set initial positions by modifying the object's attributes in OAM

    const int playerWidth  = 8,
              playerHeight = 32;

    const int playerMaxClampY = SCREEN_HEIGHT - playerHeight;
    const int playerMaxClampX = SCREEN_WIDTH - playerWidth;

    const int ballWidth  = 8,
              ballHeight = 8;

        //calculate ball velocity and position
    const int ballMaxClampX = SCREEN_WIDTH - ballWidth;
    const int ballMaxClampY = SCREEN_HEIGHT - ballHeight;

    int playerVelocity = 2;
    int ballVelocityX = 2,
        ballVelocityY = 1;
    int playerX = 5,
        playerY = 96;
    int ballX = 22,
        ballY = 96;

    int colorChangeToggle=0; //controls the color of the paddle
    int pauseToggle=0; // controls the pause state of game

    setObjectPosition(paddleAttrs, playerX,playerY);
    setObjectPosition(ballAttrs, ballX,ballY);

    unsigned short keyStates = 0;
    int frame_count= 0;
    int score = 0;

    int previous_sec=0;
    int last_frame_count = 0;

    while(1){ //loop forever
        vsync(); //skip past the rest of any current V-Blanks period & then skip past the V-Draw period
        frame_count++; //increment frame count
        iprintf("\x1b[16;0HFrames: %d!\n",frame_count);
        
        //vblankInterruptWait();

        //get current key states (REG_KEY_INPUT stores the states inverted)
        keyStates = ( (~(*reg_key_input)) & KEY_ANY); //negate input and mask for any key input

        if(keyStates & KEY_START){ //if start key is pressed, "pause" game via not updating objects
            pauseToggle = !pauseToggle;
        }

        if(!pauseToggle){

            if(keyStates & KEY_UP){ //if up key is pressed, calculate next position upwards
                playerY = clampValue(playerY - playerVelocity, 0, playerMaxClampY);
            }

            if(keyStates & KEY_DOWN){ //if down key is pressed, calculate next position downwards
                playerY = clampValue(playerY + playerVelocity, 0, playerMaxClampY);
            }

            if(keyStates & KEY_LEFT){ //if up key is pressed, calculate next position upwards
                playerX = clampValue(playerX - playerVelocity, 0, playerMaxClampX);
            }

            if(keyStates & KEY_RIGHT){ //if down key is pressed, calculate next position downwards
                playerX = clampValue(playerX + playerVelocity, 0, playerMaxClampX);
            }

            if(keyStates & KEY_UP || keyStates & KEY_DOWN || keyStates & KEY_LEFT || keyStates & KEY_RIGHT){ //if down key is pressed, calculate next position downwards
                setObjectPosition(paddleAttrs, playerX, playerY);
            }

            //check bounds of ball to see if it intersects with the player paddle
            if( (ballX >= playerX && ballX <= playerX + playerWidth) && 
                (ballY >= playerY && ballY <= playerY + playerHeight ) ){
                ballX= playerX + playerWidth; //set ball position to edge of player/paddle
                ballVelocityX = -ballVelocityX; // change X velocity to rebound the ball. Y position unchanged

                //change ball color when contact is made. Toggle between original and new color
                if(colorChangeToggle){
                    OBJECT_PALETTE_MEM[2] = RGB15(0x1F,0x1F,0x1F); // white color
                    colorChangeToggle = 0;
                } else{
                    OBJECT_PALETTE_MEM[2] = RGB15(0x1F,0x1F,0x00); // yellow color
                    colorChangeToggle = 1;
                }

            } else { //if no intersection, check if ball hits screen boundaries and correct velocity
                if(ballX == 0 || ballX == ballMaxClampX){
                    if(ballX == ballMaxClampX){
                        score++; //increase score if ball hits right side of screen
                    }
                    ballVelocityX = -ballVelocityX;
                }
                if(ballY == 0 || ballY == ballMaxClampY){
                    ballVelocityY = -ballVelocityY;
                }
            }

            ballX = clampValue(ballX+ ballVelocityX, 0, ballMaxClampX);
            ballY = clampValue(ballY+ ballVelocityY, 0, ballMaxClampY);
            setObjectPosition(ballAttrs, ballX,ballY);

            iprintf("\x1b[0;0HScore: %d\n",score);
        }

        //report number of seconds past via timer 2 value (overflow of timer 1 is approx. 1 sec)
        // clock frequency (16.78 MHz) / 256 cycles = 65,546.875, which is approximately number of ticks/increments before
        //overflow of timer register (2^16 = 65,536). Because approximation is less than actual, 
        //frames per second (FPS aka frame rate) is a little lower
        unsigned short one_sec_timer_count = *timer2_data;
        iprintf("\x1b[0;13HTimer 2: %d sec\n",one_sec_timer_count);

        if(one_sec_timer_count > previous_sec){ //if count is updated, caculate FPS
            int fps = frame_count - last_frame_count;
            iprintf("\x1b[15;15HFPS: %d\n",fps);

            //update values for next calculation
            last_frame_count = frame_count;
            previous_sec = one_sec_timer_count;
        }
    } 

    return 0;
}