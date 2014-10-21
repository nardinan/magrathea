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
#ifndef magrathea_calibrations_h
#define magrathea_calibrations_h
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "package.h"
#define d_calibrations_ladders 24
#define d_calibrations_columns 7
#define d_calibrations_directory "_cal"
#define d_calibrations_extension "cal"
typedef struct s_calibrations_environment {
	struct {
		float pedestal[d_package_channels], sigma_raw[d_package_channels], sigma[d_package_channels], pedestal_mean, pedestal_rms, sigma_raw_mean,
		      sigma_raw_rms, sigma_mean, sigma_rms;
	} ladder[d_calibrations_ladders];
} s_calibrations_environment;
extern void f_calibrations_values(struct s_calibrations_environment *environment);
extern int p_calibrations_file_read_row(char *string, float *pedestal, float *sigma_raw, float *sigma);
extern int p_calibrations_file_read(struct s_calibrations_environment *environment, FILE *stream, int ladder);
extern int p_calibrations_file(struct s_calibrations_environment *environment, const char *file);
extern int f_calibrations(struct s_calibrations_environment *environment, const char *directory);
extern void f_calibrations_export(struct s_calibrations_environment *environment, const char *destination, unsigned int trb);
#endif
