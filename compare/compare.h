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
#ifndef magrathea_compare_h
#define magrathea_compare_h
#include "interface.h"
#include "analyze.h"
#define d_compare_window_width 800
#define d_compare_window_height 600
#define d_compare_timestamp_format "%d %b %Y %H:%M:%S"
#define d_compare_timeout 150000
extern struct s_analyze_environment environment;
extern int v_selected_ladder;
extern int f_compare_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_compare_destroy(GtkWidget *widget, struct s_interface *supplied);
extern int p_compare_loop_toggle(struct s_interface *interface, int ladder, int enable);
extern int f_compare_loop(struct s_interface *interface);
#endif
