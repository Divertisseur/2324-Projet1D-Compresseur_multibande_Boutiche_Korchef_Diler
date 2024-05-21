<<<<<<< HEAD
/*
 * oledprint.c
 *
 *  Created on: May 21, 2024
 *      Author: Abdel
 */
#include "oledprint.h"
#include "stdlib.h"

int initOled(){
	ssd1306_Init();
	return ssd1306_GetDisplayOn();
}

void printSpect(const unsigned char fft[]){

	ssd1306_Fill(Black);
	for (int i =0;i<SSD1306_WIDTH;i++){
		printLine(i, 0, i, fft[i], White);

	}
	ssd1306_UpdateScreen();
	//HAL_Delay(40);

}
void printLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    ssd1306_DrawPixel(x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }

        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}


=======
/*
 * oledprint.c
 *
 *  Created on: May 21, 2024
 *      Author: Abdel
 */
#include "oledprint.h"

int initOled(){
	ssd1306_Init();
	return ssd1306_GetDisplayOn();
}

void printSpect(const unsigned char fft[]){

	ssd1306_Fill(Black);
	for (int i =0;i<SSD1306_WIDTH;i++){
		printLine(i, 0, i, fft[i], White);

	}
	ssd1306_UpdateScreen();
	HAL_Delay(40);

}
void printLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    ssd1306_DrawPixel(x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }

        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}

>>>>>>> 8b5b2b039c4d5a3d361b4b111ddc7a3c95e7f4f2
