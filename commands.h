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
#include "device.h"
extern struct s_console_command v_commands[];
extern int f_commands_trigger(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_list(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_status(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_stream(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_write(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_read(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_mask(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_focus(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_view(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_convert(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
extern int f_commands_inject(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output);
#endif
