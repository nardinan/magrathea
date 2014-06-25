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
#include "magrathea.h"
int main (int argc, char *argv[]) {
	struct s_console *console;
	struct s_console_input input = { .ready = d_true };
	struct s_console_command commands[] = {
		{
			e_console_level_guest,
			(struct s_console_parameter[]){
				{"-t", "(int) readout timeout in microseconds", d_false, d_false, d_true},
				{0}
			},
			"recv",
			"usage: recv -t <int>\n\treads output channel from the RS232 module and writes it on screen (hex)",
			&f_command_recv,
			d_true
		},{
			e_console_level_guest,
			(struct s_console_parameter[]){
				{"-x", "(string) send hexadecimal values to specified TRB (i.e. 040200)", d_false, d_false, d_true},
				{0}
			},
			"send",
			"usage: send -x <string>\n\tsends a formatted hexadecimal data to a TRB and read the formatted output",
			&f_commands_send,
			d_true
		},{
			e_console_level_guest,
			(struct s_console_parameter[]){
				{"-t", "(string) open TRB channel (i.e. /dev/ttyL0)", d_false, d_false, d_true},
				{"", "", d_false, d_false, d_false}
			},
			"open",
			"usage: open -t <string>\n\topen a stream between the machine and the TRB (replacing an already opened channel if exists)",
			&f_commands_open,
			d_true
		},{0}
	};
	f_memory_init();
	f_console_init(&console, commands, STDIN_FILENO);
	strcpy(console->prefix, "\r[input]> ");
	console->level = e_console_level_guest;
	while (d_true) {
		f_console_read(console, &input, STDOUT_FILENO, 0, 10);
		if (input.ready) {
			if ((f_string_strcmp(input.input, "quit") == 0) || (f_string_strcmp(input.input, "exit") == 0))
				break;
			f_console_execute(console, &input, STDOUT_FILENO);
		}
	}
	f_console_destroy(&console);
	f_memory_destroy();
	return 0;
}
