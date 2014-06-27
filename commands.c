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
const char *v_commands_errors[] = {
	"there isn't any open TRB connected"
};
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

d_define_command(ls) {
	char buffer[d_string_buffer_size] = {0}, backup[d_string_buffer_size], status[d_string_buffer_size];
	int index, result = 0;
	f_trb_wake_up(d_trb_common_timeout);
	for (index = 0; index < d_trb_boards; index++) {
		snprintf(backup, d_string_buffer_size, "[%s]%s TRB 0x%02x ", v_trb_boards[index].location, (v_trb_boards[index].selected)?"[*]":"",
				v_trb_boards[index].code);
		if (v_trb_boards[index].ready) {
			result++;
			snprintf(status, d_string_buffer_size, "[%sready%s]\n", v_console_styles[e_console_style_green],
					v_console_styles[e_console_style_reset]);
		} else
			snprintf(status, d_string_buffer_size, "[%soffline%s]\n", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		strncat(buffer, backup, (f_string_strlen(buffer)-d_string_buffer_size));
	}
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}

d_define_command(mask) {
	unsigned char mask = 0x00;
	char *string, buffer[d_string_buffer_size];
	int index, status;
	if ((index = d_commands_argument("-m", tokens, elements, NULL, d_console_descriptor_null)) != d_commands_argument_null) {
		string = tokens[index];
		for (index = 0; index < d_trb_boards; index++) {
			status = d_false;
			if (*string) {
				if ((*string) == '1')
					status = d_true;
				string++;
			}
			if (status)
				mask = ((unsigned char)(0x01))<<index;
		}
	} else if ((d_commands_flag("-a", tokens, elements, NULL, d_console_descriptor_null) != d_commands_argument_null) ||
			(d_commands_flag("-A", tokens, elements, NULL, d_console_descriptor_null) != d_commands_argument_null))
		mask = 0xFF;
	f_trb_apply_mask(mask);
	snprintf(buffer, d_string_buffer_size, "mask %s0x%02x%s applied\n", v_console_styles[e_console_style_bold], mask,
			v_console_styles[e_console_style_reset]);
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}

d_define_command(send) {
	unsigned char packet[d_trb_command_size], raw_command[d_trb_raw_command_size];
	char *string, singleton[(d_commands_hexadecimal_size+1)], buffer[d_string_buffer_size], backup[d_string_buffer_size];
	int result = d_false, index, index_hexadecimal, written;
	if ((index = d_commands_argument("-x", tokens, elements, "parameter '-x' not found\n", output)) != d_commands_argument_null) {
		string = tokens[index];
		singleton[d_commands_hexadecimal_size] = '\0';
		for (index = 0; index < d_trb_command_size; index++) {
			for (index_hexadecimal = 0; index_hexadecimal < d_commands_hexadecimal_size; index_hexadecimal++)
				if (*string) {
					singleton[index_hexadecimal] = *string;
					string++;
				} else
					singleton[index_hexadecimal] = 0;
			packet[index] = (unsigned char)strtol(singleton, NULL, 16);
		}
		f_trb_command_packet(raw_command, packet[0], packet[1], packet[2], packet[3]);
		f_trb_broadcast(command, d_trb_raw_command_size);
		snprintf(buffer, d_string_buffer_size, "%d bytes sent (hexadecimal input):\n", written);
		for (index = 0; index < written; index++) {
			snprintf(backup, d_string_buffer_size, "0x%02x", raw_command[index]);
			strcat(buffer, backup);
		}
		strcat(buffer, "\n");
		result = d_true;
		if (output != d_console_descriptor_null)
			write(output, buffer, f_string_strlen(buffer));
	}
	return result;
}

d_define_command(recv) {
	unsigned char input[d_string_buffer_size];
	char buffer[d_string_buffer_size], backup[d_string_buffer_size];
	int result = d_false, index, timeout, readed;
	if ((index = d_commands_argument("-t", tokens, elements, "parameter '-t' not found\n", output)) != d_commands_argument_null) {
		timeout = atoi(tokens[index]);
		if (v_trb_descriptor != d_rs232_null) {
			if ((readed = f_rs232_read_packet(v_trb_descriptor, input, d_string_buffer_size, timeout, v_trb_raw_head, v_trb_raw_tail,
							v_trb_sentinel_size)) > 0) {
				snprintf(buffer, d_string_buffer_size, "%d bytes readed (hexadecimal output):\n", readed);
				for (index = 0; index < readed; index++) {
					snprintf(backup, d_string_buffer_size, "%02x ", input[index]);
					strcat(buffer, backup);
				}
				strcat(buffer, "\n");
				result = d_true;
			} else
				snprintf(buffer, d_string_buffer_size, "stdout of slow TRB channel is %sempty%s\n", v_console_styles[e_console_style_bold],
						v_console_styles[e_console_style_reset]);
			if (output != d_console_descriptor_null)
				write(output, buffer, f_string_strlen(buffer));
		} else if (output != d_console_descriptor_null)
			write(output, v_commands_errors[e_commands_error_socket_close], f_string_strlen(v_commands_errors[e_commands_error_socket_close]));
	}
	return result;
}

