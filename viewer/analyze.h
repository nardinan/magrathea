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
#include "../clusters.h"
#define d_analyze_calibration_steps 2048
#define d_analyze_ladders 24
#define d_analyze_clusters_max 5.0f
#define d_analyze_clusters_min 3.0f
typedef struct s_analyze_data {
	struct s_event_environment compressed_event;
	float bucket[d_package_channels], adc_pedestal[d_package_channels], adc_pedestal_cn[d_package_channels], occupancy[d_package_channels],
	      occupancy_counter[d_package_channels];
	unsigned int new_bucket:1;
} s_analyze_data;
typedef struct s_analyze_calibration {
	unsigned short int package, steps;
	float bucket[d_analyze_calibration_steps][d_package_channels];
	unsigned int computed:1, drawed:1, new_bucket:1;
} s_analyze_calibration;
typedef struct s_analyze_counters {
	size_t events, data_events;
	unsigned short trigger_counter;
} s_analyze_counters;
typedef struct s_analyze_environment {
	size_t calibrated;
	time_t seconds, mseconds;
	struct s_analyze_counters counters[d_analyze_ladders];
	struct s_analyze_calibration calibration[d_analyze_ladders];
	struct s_analyze_data data[d_analyze_ladders];
	struct s_calibrations_environment computed_calibrations;
} s_analyze_environment;
extern int v_analyze_adc_pedestal, v_analyze_adc_pedestal_cn;
extern int f_analyze_data(struct s_analyze_environment *environment, unsigned short int trb, unsigned short int ladder, unsigned short int *values);
#endif
