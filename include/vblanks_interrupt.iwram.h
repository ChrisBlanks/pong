
#ifndef __VBLANKS_INTERRUPT__
#define __VBLANKS_INTERRUPT__

//defines for compiler
#define EWRAM_DATA __attribute__((section(".ewram")))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define  EWRAM_BSS __attribute__((section(".sbss")))

#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))


//declare function w/ IWRAM_CODE symbol to put codee
IWRAM_CODE void executeVBlankInterrupt(void);

#endif //__VBLANKS_INTERRUPT__