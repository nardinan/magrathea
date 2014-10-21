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
#include "../calibrations.h"
#define d_analyze_calibrations 2
#define d_analyze_pedestal_distance_max 10
#define d_analyze_csv_divisor ';'
typedef struct s_analyze_environment {
	struct s_calibrations_environment calibration[d_analyze_calibrations];
	float pedestal_distance[d_calibrations_ladders][d_package_channels];
} s_analyze_environment;
extern void f_analyze_values_write(struct s_analyze_environment *environment, FILE *stream);
extern void f_analyze_values(struct s_analyze_environment *environment);
extern int f_analyze_calibration(struct s_analyze_environment *environment, const char *reference, const char *directory);
extern void f_analyze_export(struct s_analyze_environment *environment, const char *file, int *selected);
#endif
