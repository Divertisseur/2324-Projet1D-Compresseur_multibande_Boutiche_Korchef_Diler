/*
 * RCFilter.h
 *
 *  Created on: Mar 12, 2024
 *      Author: MaelK
 */

#ifndef RCFILTER_RCFILTER_H_
#define RCFILTER_RCFILTER_H_
#include <stdio.h>
#include <stdint.h>
typedef struct {
	float coeff_A;
	float coeff_B;
	float coeff_D;
	float out_prev;
} h_RC_filter_t;

void RC_filter_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency);
float RC_filter_update(h_RC_filter_t * h_RC_filter, float input);




#endif /* RCFILTER_RCFILTER_H_ */
