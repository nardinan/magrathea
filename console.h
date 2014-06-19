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
#ifndef magrathea_console_h
#define magrathea_console_h
#include <miranda/ground.h>
#define d_console_parameter_size 5
#define d_console_command_size 16
struct s_console_command;
typedef int (*t_console_recall)(struct s_console_command *, char **, size_t);
typedef struct s_console_parameter {
	char parameter[d_console_parameter_size], description[d_string_buffer_size];
	int flag, optional;
} s_console_paramter;
typedef struct s_console_command {
	struct s_console_parameter *parameters;
	char command[d_console_command_size], description[d_string_buffer_size];
	t_console_recall recall;
} s_console_command;
#endif
