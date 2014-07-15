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
#ifndef magrathea_analyze_h
#define magrathea_analyze_h
#include <miranda/ground.h>
#include <math.h>
#include "../package.h"
extern float p_analyze_mean_f(float *values, size_t size);
extern float p_analyze_mean_i(float *values, size_t size);
extern float *f_analyze_pedestal(float values[][d_package_channels], size_t size, float *supplied);
extern float *f_analyze_sigma_raw(float values[][d_package_channels], size_t size, float *supplied);
extern float *f_analyze_cn_no_pedestal(float *no_pedestal, float sigma_multiplicator, float *sigma, float *supplied);
extern float *f_analyze_cn(float *values, float sigma_multiplicator, float *pedestal, float *sigma, float *supplied);
extern float *f_analyze_sigma(float values[][d_package_channels], size_t size, float sigma_multiplicator, float *sigma_raw, float *pedestal,
		float *supplied);
extern float *f_analyze_adc_pedestal(float values[d_package_channels], float *pedestal, float *supplied);
extern float *f_analyze_adc_pedestal_cn(float values[d_package_channels], float sigma_multiplicator, float *pedestal, float *sigma, float *supplied);
#endif
