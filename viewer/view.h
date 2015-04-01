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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include "interface.h"
#include "analyze.h"
#define d_view_calibration_steps 512
#define d_view_window_width 640
#define d_view_window_height 480
#define d_view_window_title "Magrathea event viewer"
#define d_view_timeout 100000
#define d_view_timeout_online 150
#define d_view_dampe_epoch 1356998400 /* GTM */
#define d_view_clusters_title "# timestamp seconds, mseconds, dampe_event_ID, viewer_event_ID, ladder, CoG\n"
#define d_view_rms_k 5
#define d_view_pedestal_k 2
#define d_view_bad_channels_tail "badchs"
typedef enum e_view_actions {
	e_view_action_exports_calibrations = 		0x01,
	e_view_action_exports_clusters = 		0x02,
	e_view_action_close_after_calibrations = 	0x04,
	e_view_action_filter_raw = 			0x08,
	e_view_action_filter_compressed = 		0x10,
	e_view_action_filter_download =			0x20
} e_view_actions;
typedef struct s_view_environment {
	FILE *stream, *clusters_stream;
	unsigned char buffer[d_package_buffer_size];
	char filename[PATH_MAX];
	size_t bytes;
	struct s_analyze_environment data;
} s_view_environment;
extern struct s_view_environment environment;
extern int v_view_ladder, v_view_trb, v_view_calibration_steps, v_view_calibrated, v_view_skip_frames, v_view_pause, v_view_label_refresh, v_frames, v_flags;
extern long long v_starting_time;
extern void f_view_action_dump(GtkWidget *widget, struct s_interface *supplied);
extern void f_view_action_redo(GtkWidget *widget, struct s_interface *supplied);
extern void f_view_action_last(GtkWidget *widget, struct s_interface *supplied);
extern int f_view_action_press(GtkWidget *widget, GdkEventKey *event, struct s_interface *supplied);
extern int f_view_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_view_destroy(GtkWidget *widget, struct s_interface *supplied);
extern void p_view_loop_analyze(struct s_interface *interface, unsigned short int ladder, unsigned short int *values);
extern void p_view_loop_append_signals(struct s_interface *interface, unsigned short int ladder);
extern void p_view_loop_show_signals(struct s_interface *interface);
extern void p_view_loop_read_raw(int delay);
extern void p_view_loop_read_process(struct s_interface *interface, struct s_package *package);
extern int p_view_loop_read(struct s_interface *interface, int delay);
extern int f_view_loop(struct s_interface *interface);
extern int f_view_load_channel(const char *filename);
#endif
