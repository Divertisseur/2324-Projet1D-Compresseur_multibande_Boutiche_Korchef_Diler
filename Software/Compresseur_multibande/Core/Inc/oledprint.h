/*
 * oledprint.h
 *
 *  Created on: May 21, 2024
 *      Author: saiko no oniisan
 */

#ifndef INC_OLEDPRINT_H_
#define INC_OLEDPRINT_H_

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_conf.h"
#include "ssd1306_tests.h"

int initOled();
void printSpect(const unsigned char fft[]);
void printLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);



#endif /* INC_OLEDPRINT_H_ */
