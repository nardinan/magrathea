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
#ifndef magrathea_view_h
#define magrathea_view_h
#include "interface.h"
#include "analyze.h"
#define d_view_calibration_steps 512
#define d_view_calibration_sigma_k 10.0f
#define d_view_window_width 640
#define d_view_window_height 480
typedef struct s_view_environment {
	FILE *stream;
	unsigned char buffer[d_package_buffer_size];
	size_t bytes;
	struct {
		unsigned short int package, computed:1, drawed:1;
		float bucket[d_view_calibration_steps][d_package_channels], pedestal[d_package_channels], sigma_raw[d_package_channels],
		      sigma[d_package_channels];
	} calibration;
} s_view_environment;
extern struct s_view_environment environment;
extern int v_view_ladder;
extern int f_view_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_view_destroy(GtkWidget *widget, struct s_interface *supplied);
extern void p_view_loop_analyze(struct s_interface *interface, unsigned short int *values);
extern int f_view_loop(struct s_interface *interface);
#endif
