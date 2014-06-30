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
struct s_console_command commands[] = {
	{
		e_console_level_guest,
		(struct s_console_parameter[]){{.initialized = d_false}},
		"ls",
		"usage: ls\n\tlists all trb connected (and disconnected ones) and their status",
		&f_commands_ls,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-m", "(string) mask expressed as 8 reversed bits (i.e. 10000011 selects last, first and second TRB)", d_false, d_true, d_true},
			{"-a", "[or -A] (flag) mark all TRB as selected", d_true, d_true, d_true},
			{.initialized = d_false}
		},
		"mask",
		"usage: mask -m <string of bits> | mask -A\n\tmarks TRBs that must be used during broadcasting & configuration processes",
		&f_commands_mask,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-x", "(string) send hexadecimal values to specified TRB (i.e. 0200)", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		"send",
		"usage: send -x <string>\n\tsends a formatted hexadecimal data to a TRB and read the formatted output",
		&f_commands_send,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-t", "(int) readout timeout in microseconds", d_false, d_true, d_true},
			{.initialized = d_false}
		},
		"recv",
		"usage: recv -t <int>\n\treads output channel from the RS232 module and writes it on screen (hex)",
		&f_commands_recv,
		d_true
	},{.initialized = d_false}
};

int p_magrathea_init_verbose(int descriptor, const char *subsystem) {
	char buffer[d_string_buffer_size];
	int result = d_true;
	if (descriptor != d_console_descriptor_null) {
		snprintf(buffer, d_string_buffer_size, "[^] initializing %s\"%s\"%s module\n", v_console_styles[e_console_style_green],
				subsystem, v_console_styles[e_console_style_reset]);
		write(descriptor, buffer, f_string_strlen(buffer));
		fsync(descriptor);
	}
	return result;
}

int f_magrathea_init(int descriptor) {
	int status = d_true;
	d_magrathea_module(status, descriptor, "console") {
		if ((status = f_console_init(&console, commands, STDIN_FILENO))) {
			strcpy(console->prefix, "\r[input]>");
			console->level = e_console_level_guest;
		}
	}
	d_magrathea_module(status, descriptor, "TRBs") {
		f_trb_wake_up(d_trb_common_timeout);
	}
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
