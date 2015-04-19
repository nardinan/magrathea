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
#ifndef magrathea_status_h
#define magrathea_status_h
#include "interface.h"
#include "analyze.h"
#define d_status_temperature_max 373.15f
#define d_status_temperature_min 173.15f
#define d_status_temperature_kelvin 273.15f
#define d_status_temperature_entries 10
#define d_status_temperature_wrong 221.0f
#define d_status_window_width 800
#define d_status_window_height 600
#define d_status_bucket_size 3600
#define d_status_timestamp_format "%d %b %Y %H:%M:%S"
#define d_status_timeout 20000
#define d_status_timeout_online 5000
#define d_status_skip_frames 64
typedef struct s_status_environment_values {
	time_t timestamp;
	float temperatures[e_trb_device_temperatures_null], currents[e_trb_device_currents_null], voltages[e_trb_device_voltages_null], tfh_mean;
} s_status_environment_values;
typedef struct s_status_environment {
	FILE *stream;
	unsigned char code;
	struct s_status_environment_values values[d_status_bucket_size];
	int entries;
} s_status_environment;
extern struct s_status_environment v_environment;
extern time_t v_starting_timestamp;
extern int v_frames;
extern struct s_chart_color temperature_colors[];
extern int f_status_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_status_destroy(GtkWidget *widget, struct s_interface *supplied);
extern void p_status_loop_fill_map_temperature(float value, float *R, float *G, float *B);
extern void p_status_loop_fill_map(struct s_interface *interface);
extern void p_status_loop_fill_values(void);
extern int f_status_loop(struct s_interface *interface);
#endif
