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
	char buffer[d_string_buffer_size];
	int status = d_true;
	if (descriptor != d_console_descriptor_null) {
		snprintf(buffer, d_string_buffer_size, "Magrathea - TRB data acquisition system (verision %s)\n", d_magrathea_version);
		write(descriptor, buffer, f_string_strlen(buffer));
	}
	d_magrathea_module(status, descriptor, "console") {
		if ((status = f_console_init(&console, v_commands, STDIN_FILENO))) {
			strcpy(console->prefix, "\r[?]>");
			console->level = e_console_level_guest;
		}
	}
	d_magrathea_module(status, descriptor, "TRBs") {
		f_trb_wake_up(d_trb_common_timeout);
	}
	return status;
}

int main (int argc, char *argv[]) {
	int index;
	f_memory_init();
	if (f_magrathea_init(STDOUT_FILENO)) {
		while (d_true) {
			f_console_read(console, &input, STDOUT_FILENO, 0, 10);
			if (input.ready) {
				if ((f_string_strcmp(input.input, "quit") == 0) || (f_string_strcmp(input.input, "exit") == 0))
					break;
				f_console_execute(console, &input, STDOUT_FILENO);
			}
			usleep(d_magrathea_loop_sleep);
			f_trb_acquire(d_trb_read_timeout);
		}
		for (index = 0; index < d_trb_boards; ++index)
			f_trb_disconnect(index);
		f_adlink_destroy(e_adlink_boards_trigger);
		f_adlink_destroy(e_adlink_boards_data);
		f_console_destroy(&console);
	}
	f_memory_destroy();
	return 0;
}
