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
#define d_status_bucket_size 1445
#define d_status_timestamp_format "%d %b %Y %H:%M:%S"
#define d_status_timeout 50000
#define d_status_skip_frames 15
typedef struct s_status_environment {
	FILE *stream;
	unsigned char code;
	time_t timestamp;
	struct {
		float tfh_mean[d_status_bucket_size], power_board[d_status_bucket_size], adc_board[d_status_bucket_size],
		      fpga_board_busa[d_status_bucket_size], fpga_board_busb[d_status_bucket_size];
	} temperatures;
	struct {
		float current_34[d_status_bucket_size], current_negative_33[d_status_bucket_size], current_57[d_status_bucket_size],
		      current_12[d_status_bucket_size];
	} currents;
	int entries;
} s_status_environment;
extern int v_frames;
extern struct s_status_environment environment;
extern struct s_chart_color temperature_colors[];
extern int f_status_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_status_destroy(GtkWidget *widget, struct s_interface *supplied);
extern void p_status_loop_fill_map_temperature(float value, float *R, float *G, float *B);
extern void p_status_loop_fill_map(struct s_interface *interface);
extern void p_status_loop_fill_values(void);
extern int f_status_loop(struct s_interface *interface);
#endif
