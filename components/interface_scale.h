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
#ifndef magrathea_interface_scale_h
#define magrathea_interface_scale_h
#include <gtk/gtk.h>
#include <miranda/ground.h>
#define d_interface_scale_width 900
#define d_interface_scale_height 250
#define d_max(a,b) ((a)>(b))?(a):(b)
#define d_min(a,b) ((a)<(b))?(a):(b)
struct s_chart;
typedef enum e_interface_scale_switches {
	e_interface_scale_switch_informations = 0,
	e_interface_scale_switch_NULL
} e_interface_scale_switches;
typedef enum e_interface_scale_spins {
	e_interface_scale_spin_y_top = 0,
	e_interface_scale_spin_y_bottom,
	e_interface_scale_spin_y_segments,
	e_interface_scale_spin_x_top,
	e_interface_scale_spin_x_bottom,
	e_interface_scale_spin_x_segments,
	e_interface_scale_spin_line_size,
	e_interface_scale_spin_dot_size,
	e_interface_scale_spin_NULL
} e_interface_scale_spins;
typedef enum e_interface_scale_scales {
	e_interface_scale_scale_line_R = 0,
	e_interface_scale_scale_line_G,
	e_interface_scale_scale_line_B,
	e_interface_scale_scale_background_R,
	e_interface_scale_scale_background_G,
	e_interface_scale_scale_background_B,
	e_interface_scale_scale_NULL
} e_interface_scale_scales;
typedef struct s_interface_scale {
	GtkBuilder *interface;
	GtkWindow *window;
	GtkSpinButton *spins[e_interface_scale_spin_NULL];
	GtkScale *scales[e_interface_scale_scale_NULL];
	GtkToggleButton *switches[e_interface_scale_switch_NULL];
	GtkButton *action, *export_csv, *export_png;
	struct s_chart *hooked_chart;
} s_interface_scale;
extern const char *v_interface_scale_switches[], *v_interface_scale_scales[], *v_interface_scale_spins[];
extern struct s_interface_scale *f_interface_scale_new(struct s_interface_scale *supplied, GtkBuilder *interface);
#endif

