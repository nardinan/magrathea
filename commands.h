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
#ifndef magrathea_commands_h
#define magrathea_commands_h
#include "console.h"
#include "trb.h"
#define d_define_command(sym) int f_commands_##sym(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output)
#define d_declare_command(sym) extern d_define_command(sym)
#define d_commands_argument(sym,tok,ele,msg,oput) (f_commands_get_parameter_index((sym),(tok),(ele),e_commands_parameter_argument,(msg),(oput)))
#define d_commands_flag(sym,tok,ele,msg,oput) (f_commands_get_parameter_index((sym),(tok),(ele),e_commands_parameter_flag,(msg),(oput)))
#define d_commands_argument_null -1
#define d_commands_hexadecimal_size 2
#define d_commands_buffer_size 1024
typedef enum e_commands_parameter {
	e_commands_parameter_flag,
	e_commands_parameter_argument
} e_commands_parameter;
extern int f_commands_get_parameter_index(const char *symbol, char **tokens, size_t elements, enum e_commands_parameter kind, const char *message, int output);
d_declare_command(ls);
d_declare_command(mask);
d_declare_command(send);
d_declare_command(recv);
d_declare_command(trigger);
#endif
