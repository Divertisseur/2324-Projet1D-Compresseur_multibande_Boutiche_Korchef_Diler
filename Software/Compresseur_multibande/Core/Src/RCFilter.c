/*
 * RCFilter.c
 *
 *  Created on: Mar 12, 2024
 *      Author: MaelK
 */
#include "RCFilter.h"
void RC_filter_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency){
// Calcul des coefficients
h_RC_filter->coeff_A = 1/(2*3.14159*cutoff_frequency);
h_RC_filter->coeff_B = -(1/(2*3.14159*cutoff_frequency));
h_RC_filter->coeff_D = 1/sampling_frequency;

}

float RC_filter_update(h_RC_filter_t * h_RC_filter, float input){
float output;
output = (float)((h_RC_filter->coeff_A) * input + (h_RC_filter->coeff_B) * (h_RC_filter->out_prev))/(h_RC_filter->coeff_D);
return output;
}
