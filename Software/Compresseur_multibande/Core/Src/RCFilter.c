/*
 * RCFilter.c
 *
 *  Created on: Mar 12, 2024
 *      Author: MaelK
 */
#include "RCFilter.h"

// Filtre Low pass
void LPF_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency){
    float alpha = 2 * sampling_frequency;
    float omega = 2 * 3.14159 * cutoff_frequency;
    float denom = omega + alpha;

    h_RC_filter->coeff_K = omega / denom;
    h_RC_filter->coeff_a0 = 1;
    h_RC_filter->coeff_a1 = (omega - alpha) / denom;
    h_RC_filter->coeff_b0 = omega / denom;
    h_RC_filter->coeff_b1 = omega / denom;
}

float LPF_update(h_RC_filter_t * h_RC_filter, float input){
    float output;
    output = (h_RC_filter->coeff_K * h_RC_filter->coeff_b0 * input)
           + (h_RC_filter->coeff_K * h_RC_filter->coeff_b1 * h_RC_filter->input_prev)
           - (h_RC_filter->coeff_a1 * h_RC_filter->out_prev);

    h_RC_filter->input_prev = input;
    h_RC_filter->out_prev = output;

    return output;
}

// Filtre high pass

void HPF_init(h_RC_filter_t * h_RC_filter, float cutoff_frequency, float sampling_frequency){
    float alpha = 2 * sampling_frequency;
    float omega = 2 * 3.14159 * cutoff_frequency;
    float denom = omega + alpha;

    h_RC_filter->coeff_K = alpha / denom;
    h_RC_filter->coeff_a0 = 1;
    h_RC_filter->coeff_a1 = (omega - alpha) / denom;
    h_RC_filter->coeff_b0 = alpha / denom;
    h_RC_filter->coeff_b1 = -alpha / denom;
}

float HPF_update(h_RC_filter_t * h_RC_filter, float input){
    float output;
    output = (h_RC_filter->coeff_K * h_RC_filter->coeff_b0 * input)
           + (h_RC_filter->coeff_K * h_RC_filter->coeff_b1 * h_RC_filter->input_prev)
           - (h_RC_filter->coeff_a1 * h_RC_filter->out_prev);

    h_RC_filter->input_prev = input;
    h_RC_filter->out_prev = output;

    return output;
}
