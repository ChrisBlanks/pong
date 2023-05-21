#include "gba_helper_funcs.h"
#include "gba_defines.h"


void delay(int milli_sec){
    int leftoverTime = milli_sec %1000; //get leftover time
    int seconds = (milli_sec - leftoverTime)/1000; //get number of seconds

    volatile unsigned short* timerTwoControl = (volatile unsigned short*) REG_TM2_CONTROL;
    volatile unsigned short* timerTwoCount= (volatile unsigned short*) REG_TM2_COUNT;

    volatile unsigned short* timerThreeControl = (volatile unsigned short*) REG_TM3_CONTROL;
    volatile unsigned short* timerThreeCount= (volatile unsigned short*) REG_TM3_COUNT;

    //set next value of timers
    *timerTwoCount = 0x0;
    *timerThreeCount = 0x0;

    //enable timer two & set 256 cycle mode
    *timerTwoControl = TIMER_ENABLE | TIMER_256_CYCLE ;
    //enable timer three w/ overflow
    *timerThreeControl = TIMER_ENABLE | TIMER_OVERFLOW_ENABLE;

    //wait until timer 2 overflows an equivalent number times as the number of seconds to delay
    // 1 cycle = 59.59 nanosecond, 256 cycles = 15.25504 microseconds, 1 hz / 15.25504 microseconds = 65,552 counts
    // 65,552 increments of the 256 cycle timer = 1 HZ
    while(*timerThreeCount < seconds); 

    // wait until leftover time is reached by calculating the number of counts needed to reach it
    // 256 cycles = 15.25504 microseconds, 15.25504 microseconds x 65,535 counts = .9998 secs/ 1000 = .9998 milliseconds
    while(*timerTwoCount < (65535/1000)*leftoverTime); 

    //disable timers timers
    *timerThreeControl = 0x0;
    *timerTwoControl = 0x0;
}

void vsync(){
    volatile unsigned short* vcount = (volatile unsigned short*) REG_DISPLAY_VCOUNT;

    while(*vcount >= SCREEN_HEIGHT);   // wait till VDraw
    while(*vcount < SCREEN_HEIGHT);    // wait till VBlank
}