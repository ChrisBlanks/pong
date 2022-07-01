
#ifndef __GBA_GRAPHICS__
#define __GBA_GRAPHICS__

void drawPixel(volatile unsigned short* vram_location, int x_coordinate, int y_coordinate, unsigned short color);
void drawColorFullScreen(volatile unsigned short* vram_location, unsigned short color);

#endif // ifndef __GBA_GRAPHICS__