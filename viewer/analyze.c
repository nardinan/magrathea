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
int f_analyze_calibration(struct s_analyze_environment *environment, unsigned short int ladder, unsigned short int *values) {
	int index, channel, result = d_false;
	for (index = 0; index < d_package_channels; ++index)
		environment->data[ladder].bucket[index] = values[index];
	environment->data[ladder].new_bucket = d_true;
	if (environment->calibration[ladder].package < environment->calibration[ladder].steps) {
		for (index = 0; index < d_package_channels; ++index)
			environment->calibration[ladder].bucket[environment->calibration[ladder].package][index] = values[index];
		environment->calibration[ladder].package++;
		environment->calibration[ladder].new_bucket = d_true;
	} else if (!environment->calibration[ladder].computed)  {
		f_stk_math_pedestal(environment->calibration[ladder].bucket, environment->calibration[ladder].package,
				environment->computed_calibrations.ladder[ladder].pedestal);
		f_stk_math_sigma_raw(environment->calibration[ladder].bucket, environment->calibration[ladder].package,
				environment->computed_calibrations.ladder[ladder].sigma_raw, environment->computed_calibrations.ladder[ladder].flags);
		f_stk_math_sigma(environment->calibration[ladder].bucket, environment->calibration[ladder].package, d_analyze_sigma_k,
				environment->computed_calibrations.ladder[ladder].sigma_raw, environment->computed_calibrations.ladder[ladder].pedestal,
				environment->computed_calibrations.ladder[ladder].sigma, environment->computed_calibrations.ladder[ladder].flags);
		environment->calibration[ladder].computed = d_true;
		environment->calibrated++;
		result = d_true;
	} else {
		environment->counters[ladder].data_events++;
		f_stk_math_adc_pedestal(environment->data[ladder].bucket, environment->computed_calibrations.ladder[ladder].pedestal,
				environment->data[ladder].adc_pedestal);
		f_stk_math_adc_pedestal_cn(environment->data[ladder].bucket, d_analyze_sigma_k,
				environment->computed_calibrations.ladder[ladder].pedestal, environment->computed_calibrations.ladder[ladder].sigma,
				environment->computed_calibrations.ladder[ladder].flags, environment->data[ladder].adc_pedestal_cn);
		for (channel = 0; channel < d_package_channels; ++channel)
			if (environment->computed_calibrations.ladder[ladder].flags[channel] == 0) /* no bad channel */
				if (environment->data[ladder].adc_pedestal_cn[channel] >
						(d_analyze_occupancy_k*environment->computed_calibrations.ladder[ladder].sigma[channel]))
					environment->data[ladder].occupancy[channel]++;
	}
	return result;
}
