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
#ifndef magrathea_keys_h
#define magrathea_keys_h
#include <miranda/ground.h>
typedef struct s_keys_entry {
	d_list_node_head;
	char key[d_string_buffer_size], value[d_string_buffer_size];
} s_keys_entry;
extern struct s_list *f_keys_initialize(struct s_list *supplied, const char *file, char separator);
extern void p_keys_float(struct s_list *list, const char *keys, const char postfix, float *value);
extern void p_keys_int(struct s_list *list, const char *keys, const char postfix, int *value);
extern void f_keys_destroy(struct s_list **supplied);
#endif
