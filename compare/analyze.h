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
#include <dirent.h>
#include "../package.h"
#define d_analyze_ladders 24
#define d_analyze_calibrations 2
#define d_analyze_calibration_columns 7
#define d_analyze_pedestals (d_package_channels*d_analyze_ladders)
#define d_analyze_pedestal_distance_max 10
typedef struct s_analyze_environment {
	struct {
		struct {
			float pedestal[d_package_channels], sigma_raw[d_package_channels], sigma[d_package_channels], pedestal_mean, pedestal_rms,
				sigma_raw_mean, sigma_raw_rms, sigma_mean, sigma_rms;
		} ladder[d_analyze_ladders];
	} calibration[d_analyze_calibrations];
	float pedestal_distance[d_analyze_ladders][d_package_channels];
} s_analyze_environment;
extern void f_analyze_values_write(struct s_analyze_environment *environment, FILE *stream);
extern void f_analyze_values(struct s_analyze_environment *environment);
extern int p_analyze_calibration_file_read_row(char *string, float *pedestal, float *sigma_raw, float *sigma);
extern int p_analyze_calibration_file_read(struct s_analyze_environment *environment, FILE *stream, int calibration, int ladder);
extern int p_analyze_calibration_file(struct s_analyze_environment *environment, const char *file, int calibration);
extern int p_analyze_calibration(struct s_analyze_environment *environment, const char *directory, int calibration);
extern int f_analyze_calibration(struct s_analyze_environment *environment, const char *reference, const char *directory);
#endif
