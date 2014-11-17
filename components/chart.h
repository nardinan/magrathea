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
#ifndef magrathea_chart_h
#define magrathea_chart_h
#include <math.h>
#include "keys.h"
#include "interface_scale.h"
#define d_chart_bucket 10240 /* maximum number of entries */
#define d_chart_max_nested 6
#define d_chart_font_size 7.0
#define d_chart_font_height 12.0
#define d_chart_giant_font_size 20
#define d_chart_default_font_size 10.0
#define d_same_sign(a,b) (((a)>=0)^((b)<0))
typedef enum e_chart_kinds {
	e_chart_kind_signal = 0,
	e_chart_kind_histogram,
	e_chart_kind_histogram_numeric,
	e_chart_kind_envelope
} e_chart_kinds;
typedef struct s_chart_value {
	float x, y, w;
	struct {
		float x, y, w;
		int done;
	} normalized;
} s_chart_value;
typedef struct s_chart_color {
	float R, G, B;
} s_chart_color;
typedef struct s_chart_axis {
	unsigned int segments;
	int show_negative, show_positive, show_grid;
	float range[2], minimum_distance, segments_length, offset, size;
	struct s_chart_color color;
} s_chart_axis;
typedef struct s_chart {
	GtkWidget *plane;
	cairo_t *cairo_brush;
	int head[d_chart_max_nested], last_width, last_height, show_borders, border_x, border_y, bins[d_chart_max_nested];
	enum e_chart_kinds kind[d_chart_max_nested];
	struct {
		float x_axis, y_axis;
	} normalized;
	struct s_chart_axis axis_x, axis_y;
	struct {
		float dot_size[d_chart_max_nested], line_size[d_chart_max_nested];
		struct s_chart_color color[d_chart_max_nested];
		char extension[d_chart_max_nested][d_string_buffer_size], description[d_chart_max_nested][d_string_buffer_size];
	} data;
	struct s_chart_value values[d_chart_max_nested][d_chart_bucket];
	float total[d_chart_max_nested], total_square[d_chart_max_nested], elements[d_chart_max_nested];
	struct s_interface_scale interface;
} s_chart;
extern struct s_chart *f_chart_new(struct s_chart *supplied);
extern int f_chart_hook_interface(struct s_chart *supplied, const char *interface);
extern void p_chart_style_axis(struct s_list *dictionary, const char postfix, struct s_chart_axis *axis);
extern void f_chart_style(struct s_chart *chart, const char *path);
extern void p_chart_build_bins(struct s_chart *chart, unsigned int code);
extern void f_chart_append_signal(struct s_chart *chart, unsigned int code, float x, float y);
extern void f_chart_append_histogram(struct s_chart *chart, unsigned int code, float value);
extern void f_chart_append_envelope(struct s_chart *chart, unsigned int code, float x, float max, float min);
extern void f_chart_flush(struct s_chart *chart);
extern void f_chart_denormalize(struct s_chart *chart);
extern void f_chart_integerize(struct s_chart *chart);
extern void f_chart_redraw(struct s_chart *chart);
extern void p_chart_redraw_axis_x(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height);
extern void p_chart_redraw_axis_y(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height);
extern void p_chart_redraw_grid_x(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height);
extern void p_chart_redraw_grid_y(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height);
extern void p_chart_normalize_switch(struct s_chart *chart, unsigned int code, unsigned int left, unsigned int right);
extern void p_chart_normalize_sort(struct s_chart *chart, unsigned int code);
extern void p_chart_normalize(struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height);
extern int p_chart_callback(GtkWidget *widget, GdkEvent *event, void *v_chart);
extern int p_chart_callback_scale_show(GtkWidget *widget, GdkEvent *event, struct s_interface_scale *interface);
extern int p_chart_callback_scale_action(GtkWidget *widget, struct s_interface_scale *interface);
extern int p_chart_callback_scale_hide(GtkWidget *widget, struct s_interface_scale *interfae);
#endif

