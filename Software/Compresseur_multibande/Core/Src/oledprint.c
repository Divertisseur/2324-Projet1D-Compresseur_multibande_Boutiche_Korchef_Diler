/*
 * oledprint.c
 *
 *  Created on: May 21, 2024
 *      Author: Abdel
 */
#include "oledprint.h"

void initOled(){
	ssd1306_Init();
}

int printSpect(const unsigned char fft[]){

	ssd1306_Fill(Black);
	for (int i =0;i<SSD1306_WIDTH;i++){
		ssd1306_Line(i, 0, i, fft[i], White);
	}
	ssd1306_UpdateScreen();
	HAL_Delay(40);

}


