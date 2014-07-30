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
#include "commands.h"
struct s_console_command v_commands[] = {
	{"trigger", "(usage: trigger -off | trigger -ext | trigger -s 50) defines the trigger speed and some other criterias",
		(struct s_console_parameter[]){
			{"-off", "(flag) shuts off the trigger", d_true, d_true, d_true},
			{"-ext", "(flag) sets the trigger output as a redirect of the input trigger", d_true, d_true, d_true},
			{"-s", "<50,100,200 or 300> sets the trigger to a defined speed", d_false, d_true, d_true},
			{.initialized = d_false}
		},
		&f_commands_trigger, e_console_level_guest, d_true
	},
	{"list", "(usage: list) lists all devices connected or disconnected and their current status",
		(struct s_console_parameter[]){
			{.initialized = d_false}
		},
		&f_commands_list, e_console_level_guest, d_true
	},
	{"status", "(usage: status | status -d 2) shows details about all or a specific devices (large output!)",
		(struct s_console_parameter[]){
			{"-d", "(integer) selects and shows details about a specific device", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		&f_commands_status, e_console_level_guest, d_true
	},
	{"stream", "(usage: stream -o /tmp/save_here) defines where the system have to store the output foreach TRB",
		(struct s_console_parameter[]) {
			{"-o", "(string) defines the prefix of output filename", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		&f_commands_stream, e_console_level_guest, d_true
	},
	{"write", "(usage: write -x <type><high byte><low byte>) sends a formatted hexadecimal data to a TRB and read the formatted output",
		(struct s_console_parameter[]) {
			{"-x", "(string) sets hexadecimal values to selected devices (i.e. 060000)", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		&f_commands_write, e_console_level_guest, d_true
	},
	{"read", "(usage: read) reads output channel from the RS232 module and writes it on screen (hexadecimal)",
		(struct s_console_parameter[]) {
			{.initialized = d_false}
		},
		&f_commands_read, e_console_level_guest, d_true
	},
	{"mask", "(usage: mask -m <string of bits> | mask -A) mask devices that must be used during broadcasting & configuration process",
		(struct s_console_parameter[]) {
			{"-m", "(string) mask expressed as 8 reversed bits (i.e. 11000000 selects first and second TRB)", d_false, d_true, d_true},
			{"-a", "(flag) mark all TRB as selected", d_true, d_true, d_true},
			{"-n", "(flag) mark all TRB as unselected", d_true, d_true, d_true},
			{.initialized = d_false}
		},
		&f_commands_mask, e_console_level_guest, d_true
	},
	{"focus", "(usage: focus -d 0) shows some informations about the selected device in the command line",
		(struct s_console_parameter[]) {
			{"-d", "(integer) selects and shows details about a specific device", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		&f_commands_focus, e_console_level_guest, d_true
	},
	{"view", "(usage: view -l 1) shows ADC counts and other usefull informations about a ladder of a specific TRB",
		(struct s_console_parameter[]){
			{"-l", "(integer) selects a ladder of selected devices", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		&f_commands_view, e_console_level_guest, d_true
	},
	{"convert", "(usage: convert) convert acquired files in ttree format for every masked TRB",
		(struct s_console_parameter[]){
			{.initialized = d_false}
		},
		&f_commands_convert, e_console_level_guest, d_true
	},
	{.initialized = d_false}
};
int f_commands_trigger(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_trigger, d_false, tokens, elements, output);
}

int f_commands_list(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_description, d_true, tokens, elements, output);
}

int f_commands_status(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_status, d_true, tokens, elements, output);
}

int f_commands_stream(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_stream, d_false, tokens, elements, output);
}

int f_commands_write(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_write, d_false, tokens, elements, output);
}

int f_commands_read(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_read, d_false, tokens, elements, output);
}

int f_commands_mask(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_mask, d_true, tokens, elements, output);
}

int f_commands_focus(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_focus, d_true, tokens, elements, output);
}

int f_commands_view(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_view, d_true, tokens, elements, output);
}

int f_commands_convert(struct s_console *console, struct s_console_command *command, char **tokens, size_t elements, int output) {
	return f_device_recall(e_device_calls_convert, d_true, tokens, elements, output);
}

