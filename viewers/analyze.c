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
#include "analyze.h"
float f_analyze_mean_f(float *values, size_t size) {
	float result = 0;
	int index;
	for (index = 0; index < size; index++)
		result += values[index];
	result /= (float)size;
	return result;
}

float f_analyze_mean_i(int *values, size_t size) {
	float result = 0;
	int index;
	for (index = 0; index < size; index++)
		result += values[index];
	result /= (float)size;
	return result;
}

float *f_analyze_pedestal(float values[][d_package_channels], size_t size, float *supplied) {
	float *result = supplied;
	int channel, event;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (channel = 0; channel < d_package_channels; ++channel) {
		result[channel] = 0;
		for (event = 0; event < size; event++)
			result[channel] += values[event][channel];
		result[channel] = (result[channel]/(float)size);
	}
	return result;
}

float *f_analyze_sigma_raw(float values[][d_package_channels], size_t size, float *supplied) {
	float *result = supplied, total, total_square, fraction = (1.0/(float)size);
	int channel, event;
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (channel = 0; channel < d_package_channels; channel++) {
		for (event = 0, total = 0, total_square = 0; event < size; event++) {
			total += values[event][channel];
			total_square += values[event][channel]*values[event][channel];
		}
		total *= fraction;
		total_square *= fraction;
		result[channel] = sqrt(fabs(total_square-(total*total)));
	}
	return result;
}

float *f_analyze_cn_no_pedestal(float *no_pedestal, float sigma_multiplicator, float *sigma, int *flags, float *supplied) {
	float elements[d_package_channels_on_va];
	int va, channel, local_channel, entries, index;
	for (va = 0, channel = 0; va < d_package_vas; va++, channel += d_package_channels_on_va) {
		supplied[va] = 0;
		for (local_channel = channel, entries = 0; local_channel < (channel+d_package_channels_on_va); local_channel++) {
			for (index = (entries-1); ((index >= 0) && (elements[index] > no_pedestal[local_channel])); index--)
				elements[index+1] = elements[index];
			elements[index+1] = no_pedestal[local_channel];
			entries++;
		}
		supplied[va] = (elements[(entries/2)]+elements[(entries/2)-1])/2.0f;
	}
	return supplied;
}

float *f_analyze_cn(float *values, float sigma_multiplicator, float *pedestal, float *sigma, int *flags, float *supplied) {
	float no_pedestal[d_package_channels];
	int index;
	for (index = 0; index < d_package_channels; index++)
		no_pedestal[index] = values[index]-pedestal[index];
	return f_analyze_cn_no_pedestal(no_pedestal, sigma_multiplicator, sigma, flags, supplied);
}

float *f_analyze_sigma(float values[][d_package_channels], size_t size, float sigma_multiplicator, float *sigma_raw, float *pedestal, int *flags,
		float *supplied) {
	float *result = supplied, common_noise[d_package_vas], common_noise_pure[d_package_channels] = {0},
	common_noise_pure_square[d_package_channels] = {0}, value, fraction;
	int channel, event, local_entries[d_package_channels] = {0};
	if (!result)
		if (!(result = (float *) d_malloc(sizeof(float)*d_package_channels)))
			d_die(d_error_malloc);
	for (event = 0; event < size; event++) {
		f_analyze_cn(values[event], sigma_multiplicator, pedestal, sigma_raw, flags, common_noise);
		for (channel = 0; channel < d_package_channels; channel++) {
			value = ((float)(values[event][channel]))-pedestal[channel]-common_noise[(int)(channel/d_package_channels_on_va)];
			if (fabs(value) < (sigma_multiplicator*sigma_raw[channel])) {
				common_noise_pure[channel] += value;
				common_noise_pure_square[channel] += value*value;
				local_entries[channel]++;
			}
		}
	}
	for (channel = 0; channel < d_package_channels; channel++)
		if (local_entries[channel]) {
			fraction = (1.0/(float)local_entries[channel]);
			common_noise_pure[channel] *= fraction;
			common_noise_pure_square[channel] *= fraction;
			result[channel] = sqrt(fabs(common_noise_pure_square[channel]-(common_noise_pure[channel]*common_noise_pure[channel])));
		}
	return result;
}


