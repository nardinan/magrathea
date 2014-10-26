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
#ifndef magrathea_stk_math_h
#define magrathea_stk_math_h
#include <miranda/ground.h>
#include <math.h>
#include "package.h"
#include "trb_device.h"
#define d_stk_math_sigma_k 10.0
#define d_stk_math_sigma_k_max 2.0
#define d_stk_math_sigma_min 1.0
#define d_stk_math_sigma_max 6.0
#define d_stk_math_sigma_raw_min 1.0
#define d_stk_math_sigma_raw_max 15.0
typedef enum e_stk_math_flags {
	e_stk_math_flag_bad = 			0x01,
	e_stk_math_flag_bad_sigma = 		0x02,
	e_stk_math_flag_bad_sigma_raw_range = 	0x04,
	e_stk_math_flag_bad_sigma_range = 	0x08
} e_stk_math_flags;
extern float *f_stk_math_pedestal(float values[][d_package_channels], size_t size, float *supplied);
extern float *f_stk_math_sigma_raw(float values[][d_package_channels], size_t size, float *supplied, unsigned short *supplied_flags);
extern float *f_stk_math_cn_no_pedestal(float *no_pedestal, float sigma_multiplicator, float *sigma, float *supplied);
extern float *f_stk_math_cn(float *values, float sigma_multiplicator, float *pedestal, float *sigma, float *supplied);
extern float *f_stk_math_sigma(float values[][d_package_channels], size_t size, float sigma_multiplicator, float *sigma_raw, float *pedestal,
		float *supplied_values, unsigned short *supplied_flags);
extern float *f_stk_math_adc_pedestal(float values[d_package_channels], float *pedestal, float *supplied);
extern float *f_stk_math_adc_pedestal_cn(float values[d_package_channels], float sigma_multiplicator, float *pedestal, float *sigma, unsigned short *flags,
		float *supplied);
#endif
