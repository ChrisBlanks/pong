#include "gba_defines.h"
#include "resources/test.h"

#include "vblanks_interrupt.iwram.h"


IWRAM_CODE void executeVBlankInterrupt(void){

    volatile unsigned short* interrupt_master = (volatile unsigned short*)INTERRUPT_MASTER_ENABLE;
    volatile unsigned short* interrupt_status = (volatile unsigned short*)INTERRUPT_REQUEST_STATUS;
    volatile unsigned short* interrupt_check_flag = (volatile unsigned short*)INTERRUPT_CHECK_FLAG;

    //define dma registers
    volatile unsigned int* dma_1_source = (volatile unsigned int*) DMA1_SOURCE_ADDR_L;
    volatile unsigned int* dma_1_destination = (volatile unsigned int*) DMA1_DESTINATION_ADDR_L;
    volatile unsigned int* dma_1_control = (volatile unsigned int*) DMA1_WORD_COUNT;

    *interrupt_master = 0; //disable master interrupt
    unsigned short current_status = *interrupt_status;


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
    *interrupt_master = 1; //enable master interrupt
}