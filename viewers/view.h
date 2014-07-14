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
#include "../package.h"
extern FILE *v_stream;
extern unsigned char v_buffer[d_package_buffer_size];
extern size_t v_bytes;
extern int v_view_ladder;
extern int f_view_initialize(struct s_interface *supplied, const char *builder_path);
extern void f_view_destroy(GtkWidget *widget, struct s_interface *supplied);
extern int f_view_loop(struct s_interface *interface);
#endif
