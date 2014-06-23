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
int v_magrathea_descriptor = d_console_descriptor_null;
int f_commands_get_parameter_index(const char *symbol, char **tokens, size_t elements, enum e_commands_parameter kind, const char *message, int output) {
	int result = d_commands_argument_null, index;
	for (index = 0; index < elements; index++)
		if (f_string_strcmp(tokens[index], symbol) == 0) {
			if (kind == e_commands_parameter_flag)
				result = index;
			else if ((kind == e_commands_parameter_argument) && ((index+1) < elements))
				result = (index+1);
			break;
		}
	if ((result == -1) && (message) && (output != d_console_descriptor_null))
		write(output, message, f_string_strlen(message));
	return result;
}

d_define_command(open) {
	char *channel, buffer[d_string_buffer_size];
	int result = d_false, index;
	if ((index = d_commands_argument("-t", tokens, elements, "parameter '-t' not found\n", output)) != d_commands_argument_null) {
		channel = tokens[index];
		if (v_magrathea_descriptor != d_console_descriptor_null) {
			f_rs232_close(v_magrathea_descriptor);
			v_magrathea_descriptor = d_console_descriptor_null;
		}
		if ((result = f_rs232_open(channel, e_rs232_baud_115200, e_rs232_bits_8, e_rs232_stops_2_bit, e_rs232_parity_odd, d_false,
						&v_magrathea_descriptor, NULL)))
			snprintf(buffer, d_string_buffer_size, "channel %s has been opened\n", channel);
		else
			snprintf(buffer, d_string_buffer_size, "%sYECK!%s unable to open channel %s\n", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset], channel);
		if (output != d_console_descriptor_null)
			write(output, buffer, f_string_strlen(buffer));
	}
	return result;
}

d_define_command(send) {
	int result = 0;
	/* not yet implemented */
	return result;
}
