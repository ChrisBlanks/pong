#include "gba_graphics.h"
#include "gba_defines.h"


void drawPixel(volatile unsigned short* vram_location, int x_coordinate, int y_coordinate, unsigned short color){
    int x_adjusted = 0, 
        y_adjusted = 0;
    int index = 0;

    if(x_coordinate >= SCREEN_WIDTH ){
        x_adjusted = SCREEN_WIDTH - 1; //max index value for X
    } else if(x_coordinate < 0){
        x_adjusted = 0;
    } else{
        x_adjusted = x_coordinate;
    }

    if(y_coordinate >= SCREEN_HEIGHT ){
        y_adjusted = SCREEN_HEIGHT - 1; //max index value for y
    } else if(y_coordinate < 0){
        y_adjusted = 0;
    } else{
        y_adjusted = y_coordinate;
    }

    index = y_adjusted*SCREEN_WIDTH + x_adjusted;
    
    vram_location[index] = color; //assign color to pixel index
}

void drawColorFullScreen(volatile unsigned short* vram_location, unsigned short color){
    
    //write color to every pixel in the screen (160x240)
    for(int index=0; index < SCREEN_HEIGHT*SCREEN_WIDTH ; index++){
        vram_location[index] = color;
    }

}