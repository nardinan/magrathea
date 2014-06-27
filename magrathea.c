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
struct s_console *console;
struct s_console_input input = { .ready = d_true };
struct s_console_commans commands[] = {
	{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-t", "(int) readout timeout in microseconds", d_false, d_false, d_true},{0}
		},
		"recv",
		"usage: recv -t <int>\n\treads output channel from the RS232 module and writes it on screen (hex)",
		&f_commands_recv,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-x", "(string) send hexadecimal values to specified TRB (i.e. 040200)", d_false, d_false, d_true},{0}
		},
		"send",
		"usage: send -x <string>\n\tsends a formatted hexadecimal data to a TRB and read the formatted output",
		&f_commands_send,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-t", "(string) open TRB channel (i.e. /dev/ttyL0)", d_false, d_false, d_true},{0}
		},
		"open",
		"usage: open -t <string>\n\topen a stream between the machine and the TRB (replacing an already opened channel if exists)",
		&f_commands_open,
		d_true
	},{0}
};

void p_magrathea_init_verbose(int descriptor, const char *subsystem) {
	char buffer[d_string_buffer_size];
	if (descriptor != d_console_descriptor_null) {
		snprintf(buffer, d_string_buffer_size, "[^] initializing %s\"%s\"%s\n module", v_console_styles[e_console_style_green],
				subsystem, v_console_styles[e_console_style_reset]);
		write(descriptor, buffer, f_string_strlen(buffer));
		fsync(descriptor);
	}
}

int f_magrathea_init(int descriptor) {
	int status = d_true;
	d_magrathea_module_begin(status, descriptor, "console") {
		if ((status = f_console_init(&console, commands, STDIN_FILENO))) {
			strcpy(console->prefix, "\r[input]>");
			console->level = e_console_level_guest;
		}
	} d_magrathea_module_end;
	d_magrathea_module_begin(status, descriptor, "TRBs") {
		f_trb_wake_up(d_trb_common_timeout);
	} d_magrathea_module_end;
	return status;
}


int main (int argc, char *argv[]) {
	f_memory_init();
	if (f_magrathea_init(STDOUT_FILENO)) {
		while (d_true) {
			f_console_read(console, &input, STDOUT_FILENO, 0, 10);
			if (input.ready) {
				if ((f_string_strcmp(input.input, "quit") == 0) || (f_string_strcmp(input.input, "exit") == 0))
					break;
				f_console_execute(console, &input, STDOUT_FILENO);
			}
		}
		f_console_destroy(&console);
	}
	f_memory_destroy();
	return 0;
}
