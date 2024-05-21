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

void initOled();
int printSpect(const unsigned char fft[]);



#endif /* INC_OLEDPRINT_H_ */
