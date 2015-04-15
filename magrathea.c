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
const char *quit_greetings[] = {
	"ciao ciao!\n", 	/* italian */
	"bye bye!\n",		/* english */
	"au revoir!\n",		/* french  */
	"zaijian!\n"		/* chinese */
};
struct s_device v_initialized_devices[] = {
	{0x00, "TRB board #1",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x01, "TRB board #2",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x02, "TRB board #3",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x03, "TRB board #4",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x04, "TRB board #5",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x05, "TRB board #6",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x06, "TRB board #7",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0x07, "TRB board #8",
		(t_device_call[]){NULL, &f_trb_device_description, &f_trb_device_status, &f_trb_device_stream, &f_trb_device_write, NULL, &f_trb_device_mask,
			&f_trb_device_focus, &f_trb_device_view, &f_trb_device_convert, &f_trb_device_inject},
		(t_device_call_generic[]){&f_trb_device_enabled, &f_trb_device_initialize, &f_trb_device_destroy},
		&f_trb_device_refresh
	},
	{0xaa, "Trigger board",
		(t_device_call[]){&f_trigger_device_trigger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
		(t_device_call_generic[]){NULL, &f_trigger_device_initialize, &f_trigger_device_destroy},
		NULL
	},
	{0xbb, "LVDS board",
		(t_device_call[]){NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
		(t_device_call_generic[]){NULL, &f_lvds_device_initialize, &f_lvds_device_destroy},
		&f_lvds_device_refresh
	},
	{0xcc, "Telnet server",
		(t_device_call[]){NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
		(t_device_call_generic[]){NULL, &f_telnet_device_initialize, &f_telnet_device_destroy},
		&f_telnet_device_refresh
	}, {0xff}
};
struct s_console *console;
struct s_console_input input = { .ready = d_true };
void p_magrathea_sigpipe_ignore(int signal) {
	/* do nothing */
}

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
	int index, status = d_true;
	if (descriptor != d_console_descriptor_null) {
		snprintf(buffer, d_string_buffer_size, "Magrathea - TRB data acquisition system (version %s)\n", d_magrathea_version);
		write(descriptor, buffer, f_string_strlen(buffer));
	}
	d_magrathea_module(status, descriptor, "console") {
		if ((status = f_console_init(&console, v_commands, STDIN_FILENO))) {
			strcpy(console->prefix, "\r[input]>");
			console->level = e_console_level_guest;
			console->input_enabled = d_true;
		}
	}
	d_magrathea_module(status, descriptor, "devices") {
		v_devices = v_initialized_devices;
		if (descriptor != d_console_descriptor_null) {
			for (index = 0; v_devices[index].code != 0xff; index++) {
				snprintf(buffer, d_string_buffer_size, "\t[required unity] name: %s%s%s\tcode: 0x%02x\n",
						v_console_styles[e_console_style_bold], v_devices[index].description, v_console_styles[e_console_style_reset],
						v_devices[index].code);
				write(descriptor, buffer, f_string_strlen(buffer));
			}
			fsync(descriptor);
		}
		f_device_system_recall(e_device_system_calls_initialize, d_console_descriptor_null);
	}
	return status;
}

void f_magrathea_greetings(int descriptor) {
	int index, value;
	if (descriptor != d_console_descriptor_null) {
		for (index = 0; quit_greetings[index]; index++);
		srand(time(NULL));
		value = (rand()%index);
		write(descriptor, quit_greetings[value], f_string_strlen(quit_greetings[value]));
		fsync(descriptor);
	}
}

int main (int argc, char *argv[]) {
	f_memory_init();
	if (f_magrathea_init(STDOUT_FILENO)) {
		signal(SIGPIPE, p_magrathea_sigpipe_ignore);
		while (d_true) {
			f_console_read(console, &input, STDOUT_FILENO, 0, 10);
			if (input.ready) {
				if (f_string_strcmp(input.input, d_magrathea_exit_command) == 0)
					break;
				f_console_execute(console, &input, STDOUT_FILENO);
			}
			usleep(d_magrathea_loop_sleep);
			f_device_system_refresh(console);
		}
		f_device_system_recall(e_device_system_calls_destroy, STDOUT_FILENO);
		f_magrathea_greetings(STDOUT_FILENO);
		f_console_destroy(&console);
	}
	f_memory_destroy();
	return 0;
}
