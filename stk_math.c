/*
 * magrathea
 * Copyright (C) 2014 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "stk_math.h"
float *f_stk_math_pedestal(float values[][d_package_channels], size_t size, float *supplied) {
	float *result = supplied;
	int channel, event;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (channel = 0; channel < d_package_channels; ++channel) {
		for (event = 0; event < size; ++event)
			result[channel] += values[event][channel];
		result[channel] = (result[channel]/(float)size);
	}
	return result;
}

float *f_stk_math_sigma_raw(float values[][d_package_channels], size_t size, float *supplied, unsigned short *supplied_flags) {
	float *result = supplied, total, total_square;
	int channel, event;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (channel = 0; channel < d_package_channels; ++channel) {
		for (event = 0, total = 0, total_square = 0; event < size; ++event) {
			total += values[event][channel];
			total_square += values[event][channel]*values[event][channel];
		}
		total = (total/(float)size);
		total_square = (total_square/(float)size);;
		result[channel] = sqrt(fabs(total_square-(total*total)));
	}
	if (supplied_flags)
		for (channel = 0; channel < d_package_channels; ++channel)
			if ((result[channel] > d_stk_math_sigma_raw_max) || (result[channel] < d_stk_math_sigma_raw_min))
				supplied_flags[channel] |= e_stk_math_flag_bad_sigma_raw_range;
	return result;
}

float *f_stk_math_cn_no_pedestal(float *no_pedestal, float sigma_multiplicator, float *sigma, float *supplied) {
	float elements[d_package_channels_on_va];
	int va, channel, local_channel, entries, index;
	for (va = 0, channel = 0; va < d_package_vas; va++, channel += d_package_channels_on_va) {
		supplied[va] = 0;
		for (local_channel = channel, entries = 0; local_channel < (channel+d_package_channels_on_va); ++local_channel) {
			for (index = (entries-1); ((index >= 0) && (elements[index] > no_pedestal[local_channel])); --index)
				elements[index+1] = elements[index];
			elements[index+1] = no_pedestal[local_channel];
			entries++;
		}
		supplied[va] = (elements[(entries/2)]+elements[(entries/2)-1])/2.0f;
	}
	return supplied;
}

float *f_stk_math_cn(float *values, float sigma_multiplicator, float *pedestal, float *sigma, float *supplied) {
	float no_pedestal[d_package_channels];
	f_stk_math_adc_pedestal(values, pedestal, no_pedestal);
	return f_stk_math_cn_no_pedestal(no_pedestal, sigma_multiplicator, sigma, supplied);
}

float *f_stk_math_sigma(float values[][d_package_channels], size_t size, float sigma_multiplicator, float *sigma_raw, float *pedestal, float *supplied_values,
		unsigned short *supplied_flags) {
	float *result = supplied_values, common_noise[d_package_vas], common_noise_pure[d_package_channels] = {0},
		common_noise_pure_square[d_package_channels] = {0}, value, fraction, mean = 0;
	int channel, event, local_entries[d_package_channels] = {0};
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (event = 0; event < size; ++event) {
		f_stk_math_cn(values[event], sigma_multiplicator, pedestal, sigma_raw, common_noise);
		for (channel = 0; channel < d_package_channels; channel++) {
			value = ((float)(values[event][channel]))-pedestal[channel]-common_noise[(int)(channel/d_package_channels_on_va)];
			if (fabs(value) < (sigma_multiplicator*sigma_raw[channel])) {
				common_noise_pure[channel] += value;
				common_noise_pure_square[channel] += value*value;
				local_entries[channel]++;
			}
		}
	}
	for (channel = 0; channel < d_package_channels; ++channel)
		if (local_entries[channel]) {
			fraction = (1.0/(float)local_entries[channel]);
			common_noise_pure[channel] *= fraction;
			common_noise_pure_square[channel] *= fraction;
			result[channel] = sqrt(fabs(common_noise_pure_square[channel]-(common_noise_pure[channel]*common_noise_pure[channel])));
			mean += result[channel];
		}
	if (supplied_flags) {
		mean /= (float)d_package_channels;
		for (channel = 0; channel < d_package_channels; ++channel) {
			if (result[channel] > (d_stk_math_sigma_k_max*mean))
				supplied_flags[channel] |= e_stk_math_flag_bad_sigma;
			if ((result[channel] > d_stk_math_sigma_max) || (result[channel] < d_stk_math_sigma_min))
				supplied_flags[channel] |= e_stk_math_flag_bad_sigma_range;
		}
	}
	return result;
}

float *f_stk_math_adc_pedestal(float values[d_package_channels], float *pedestal, float *supplied) {
	float *result = supplied;
	int index;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (index = 0; index < d_package_channels; ++index)
		supplied[index] = values[index]-pedestal[index];
	return result;
}

float *f_stk_math_adc_pedestal_cn(float values[d_package_channels], float sigma_multiplicator, float *pedestal, float *sigma, unsigned short *flags,
		float *supplied, float *supplied_common_noise) {
	float *result = supplied, common_noise[d_package_vas];
	int index;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	f_stk_math_cn(values, sigma_multiplicator, pedestal, sigma, common_noise);
	for (index = 0; index < d_package_channels; ++index)
		if ((flags) && (flags[index] != e_stk_math_flag_ok))
			supplied[index] = 0;
		else
			supplied[index] = values[index]-pedestal[index]-common_noise[(index/d_package_channels_on_va)];
	if (supplied_common_noise)
		memcpy(supplied_common_noise, common_noise, (sizeof(float)*d_package_vas));
	return result;
}

