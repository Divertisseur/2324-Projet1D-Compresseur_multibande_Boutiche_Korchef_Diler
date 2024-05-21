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
	float coeff_K;
	float coeff_a0;
	float coeff_a1;
	float coeff_b0;
	float coeff_b1;
	float out_prev;
	float input_prev;
} h_RC_filter_t;


void LPF_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency);
float LPF_update(h_RC_filter_t * h_RC_filter, float input);
void HPF_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency);
float HPF_update(h_RC_filter_t * h_RC_filter, float input);




#endif /* RCFILTER_RCFILTER_H_ */
