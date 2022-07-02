/*
 * Author: ChrisB
 * Purpose: Pong game
 */
 
#define __GBA_VARIABLES_MAIN__
#include "gba_defines.h"


#include "gba_graphics.h"
#include "gba_helper_funcs.h"
#include "test.h"

#include "vblanks_interrupt.iwram.h"

//sample rate of test audio sample
#define AUDIO_SAMPLE_RATE 44100


//global variables

//unsigned int CHANNEL_A_VBLANKS_REMAINING = 0; //counts remaining VBlanks before sound playback is finished
//unsigned int CHANNEL_A_VBLANKS_TOTAL = 0; //number of required VBlanks for sound to play

/// define the timer data and control registers 
volatile unsigned short* timer0_data = (volatile unsigned short*) REG_TM0_COUNT;
volatile unsigned short* timer0_control = (volatile unsigned short*) REG_TM0_CONTROL;

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

//sound functions

void vblankInterruptWait(){
    asm volatile("swi 0x05"); //0x05 is VblankIntrWait in the GBA function table
}

/*
void executeVBlankInterrupt(void){
    //*interrupt_master = 0; //disable master interrupt
    unsigned short current_status = *interrupt_status;

    OBJECT_PALETTE_MEM[2] = RGB15(0x00,0x1F,0x00); // green color
    if( ( *interrupt_status & INTERRUPT_VBLANK) == INTERRUPT_VBLANK){ //check if VBlank interrupt was triggered
        if(CHANNEL_A_VBLANKS_REMAINING <= 0){ //if no more vblanks remain, perform action
            
            CHANNEL_A_VBLANKS_REMAINING = CHANNEL_A_VBLANKS_TOTAL;
            *dma_1_control = 0x0; //disable DMA before making changes
            *dma_1_source = (unsigned int)  test_sound_data; //set dma source value to address of test sound data
            *dma_1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER_0 | DMA_ENABLE; //reenable dma1   

        } else{ //decrement num of vblanks remaining
            CHANNEL_A_VBLANKS_REMAINING--;
        }

    }

    *interrupt_status = INTERRUPT_VBLANK; //restore status
    *interrupt_check_flag |= INTERRUPT_VBLANK;
    //*interrupt_master = 1; //enable master interrupt
}

*/

void setupSoundTimers(){
    *timer0_control = 0x0; //disable timer 0

    unsigned short ticks_per_sample = CLOCK_HZ / AUDIO_SAMPLE_RATE; //divide clock speed (~16.78 MHz) by audio sample rate
    *timer0_data = 65536 - ticks_per_sample;

    CHANNEL_A_VBLANKS_REMAINING = test_bytes * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
    CHANNEL_A_VBLANKS_TOTAL = CHANNEL_A_VBLANKS_REMAINING;

    *timer0_control = TIMER_ENABLE | TIMER_1_CYCLE;
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
    *dma_1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER_0 | DMA_ENABLE;
}



int main(void){
    
    *interrupt_master = 0; //disable master interrupt

    *interrupt_callback_addr = (unsigned int) &executeVBlankInterrupt; //set jump address to callback func memory location
    *display_reg_status |= REG_DISPLAY_STATUS_VBLANK_INTERRUPT_ENABLE; //enable vblank interrupt in register
    *interrupt_individual |= INTERRUPT_VBLANK; //enable VBLANK interrupt
    
    *interrupt_master = 0x1; //enable master interrupt

    *sound_reg = 0x0; //clear sound control 
    
    setupSoundTest();
    setupSoundTimers();
    
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

    const int ballWidth  = 8,
              ballHeight = 8;

    int playerVelocity = 2;
    int ballVelocityX = 2,
        ballVelocityY = 1;
    int playerX = 5,
        playerY = 96;
    int ballX = 22,
        ballY = 96;

    int colorChangeToggle=0;

    setObjectPosition(paddleAttrs, playerX,playerY);
    setObjectPosition(ballAttrs, ballX,ballY);

    //set the display parameters to enable objects. Use a 1D object->tile mapping
    *reg_display = 0x1000 | 0x0040; //object mode & OBJ Character VRAM Mapping: 1-dim & BG0

    unsigned short keyStates = 0;
    while(1){ //loop forever
        vsync(); //skip past the rest of any current V-Blanks period & then skip past the V-Draw period
        
        //vblankInterruptWait();

        //get current key states (REG_KEY_INPUT stores the states inverted)
        keyStates = ( (~(*reg_key_input)) & KEY_ANY); //negate input and mask for any key input

        int playerMaxClampY = SCREEN_HEIGHT - playerHeight;
        int playerMaxClampX = SCREEN_WIDTH - playerWidth;

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

        //calculate ball velocity and position
        int ballMaxClampX = SCREEN_WIDTH - ballWidth,
            ballMaxClampY = SCREEN_HEIGHT - ballHeight;

        //check bounds of ball to see if it intersects with the player paddle
        if( (ballX >= playerX && ballX <= playerX + playerWidth) && 
            (ballY >= playerY && ballY <= playerY + playerHeight ) ){
            ballX= playerX + playerWidth; //set ball position to edge of player/paddle
            ballVelocityX = -ballVelocityX; // change X velocity to rebound the ball. Y position unchanged

            //change ball color when contact is made. Toggle between original and new color
            if(colorChangeToggle){
                OBJECT_PALETTE_MEM[1] = RGB15(0x1F,0x1F,0x1F); // white color
                colorChangeToggle = 0;
            } else{
                OBJECT_PALETTE_MEM[1] = RGB15(0x1F,0x1F,0x00); // yellow color
                colorChangeToggle = 1;
            }

        } else { //if no intersection, check if ball hits screen boundaries and correct velocity
            if(ballX == 0 || ballX == ballMaxClampX){
                ballVelocityX = -ballVelocityX;
            }
            if(ballY == 0 || ballY == ballMaxClampY){
                ballVelocityY = -ballVelocityY;
            }
        }

        ballX = clampValue(ballX+ ballVelocityX, 0, ballMaxClampX);
        ballY = clampValue(ballY+ ballVelocityY, 0, ballMaxClampY);
        setObjectPosition(ballAttrs, ballX,ballY);
    } 

    return 0;
}