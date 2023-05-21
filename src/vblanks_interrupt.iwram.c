#include "gba_defines.h"
#include "pong.h"

#include "vblanks_interrupt.iwram.h"


IWRAM_CODE void executeVBlankInterrupt(void){

    volatile unsigned short* interrupt_master = (volatile unsigned short*)INTERRUPT_MASTER_ENABLE;  //REG_IME
    volatile unsigned short* interrupt_status = (volatile unsigned short*)INTERRUPT_REQUEST_STATUS; //REG_IF
    volatile unsigned short* interrupt_check_flag = (volatile unsigned short*)INTERRUPT_CHECK_FLAG; //REG_IFBIOS

    //define dma registers
    volatile unsigned int* dma_1_source = (volatile unsigned int*) DMA1_SOURCE_ADDR_L;
    //volatile unsigned int* dma_1_destination = (volatile unsigned int*) DMA1_DESTINATION_ADDR_L;
    volatile unsigned int* dma_1_control = (volatile unsigned int*) DMA1_WORD_COUNT;

    *interrupt_master = 0; //disable master interrupt
    unsigned short current_status = *interrupt_status;


    if( ( *interrupt_status & INTERRUPT_VBLANK) == INTERRUPT_VBLANK){ //check if VBlank interrupt was triggered
        if(CHANNEL_A_VBLANKS_REMAINING <= 0){ //if no more vblanks remain, reset data source in order to loop
            
            CHANNEL_A_VBLANKS_REMAINING = CHANNEL_A_VBLANKS_TOTAL;
            *dma_1_control = 0x0; //disable DMA before making changes
            *dma_1_source = (unsigned int) pong; //set dma source value to address of test sound data
            *dma_1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER_0 | DMA_ENABLE; //reenable dma1   

        } else{ //decrement num of vblanks remaining
            CHANNEL_A_VBLANKS_REMAINING--;
        }

    }

    *interrupt_status = current_status; //restore status
    *interrupt_check_flag |= INTERRUPT_VBLANK;
    *interrupt_master = 1; //enable master interrupt
}