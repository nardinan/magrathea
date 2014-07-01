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
	{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{.initialized = d_false}
		},
		"ls",
		"(usage: ls) lists all trb connected (and disconnected ones) and their current status",
		&f_commands_ls,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-m", "(string) mask expressed as 8 reversed bits (i.e. 11000000 selects first and second TRB)", d_false, d_true, d_true},
			{"-a", "[or -A] (flag) mark all TRB as selected", d_true, d_true, d_true},
			{.initialized = d_false}
		},
		"mask",
		"(usage: mask -m <string of bits> | mask -A) marks TRBs that must be used during broadcasting & configuration processes",
		&f_commands_mask,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-x", "(string) send hexadecimal values to specified TRB (i.e. 020200)", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		"send",
		"(usage: send -x <string>) sends a formatted hexadecimal data to a TRB and read the formatted output",
		&f_commands_send,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-t", "(int) readout timeout in microseconds", d_false, d_true, d_true},
			{.initialized = d_false}
		},
		"recv",
		"(usage: recv -t <int>) reads output channel from the RS232 module and writes it on screen (hex)",
		&f_commands_recv,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-off", "(flag) shuts off the trigger board output", d_false, d_true, d_true},
			{"-ext", "(flag) sets the trigger board output as a redirect of the input trigger", d_false, d_true, d_true},
			{"-s", "<50,100,200 or 300> sets the trigger to a certain speed", d_false, d_true, d_true},
			{.initialized = d_false}
		},
		"trigger",
		"(usage: trigger -off | trigger -s 50) defines the trigger board speed and criterias",
		&f_commands_trigger,
		d_true
	},{
		e_console_level_guest,
		(struct s_console_parameter[]){
			{"-o", "(string) define the prefix of output filenames (i.e. /tmp/output)", d_false, d_false, d_true},
			{.initialized = d_false}
		},
		"store",
		"(usage: store -o /tmp/cosmics_10.7.14) defines where the system have to store the output foreach TRB",
		&f_commands_store,
		d_true
	},{.initialized = d_false}
};
const char *v_commands_bytes_extensions[] = {
	"B",
	"kB",
	"MB",
	"GB",
	"TB",
	"YB",
	NULL
};
int f_commands_get_parameter_index(const char *symbol, char **tokens, size_t elements, enum e_commands_parameter kind, const char *message, int output) {
	int result = d_commands_argument_null, index;
	for (index = 0; index < elements; ++index)
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

const char *f_commands_bytes_extension(float *value) {
	int result;
	for (result = 0; v_commands_bytes_extensions[result+1]; result++)
		if ((*value) >= 1024.0)
			(*value) /= 1024.0;
		else
			break;
	return v_commands_bytes_extensions[result];
}

d_define_command(ls) {
	const char *postfix;
	char buffer[d_commands_buffer_size] = {0}, backup[d_string_buffer_size], status[d_string_buffer_size], stream[d_string_buffer_size];
	int result = d_false, index;
	float value;
	f_trb_wake_up(d_trb_common_timeout);
	for (index = 0; index < d_trb_boards; ++index) {
		snprintf(backup, d_string_buffer_size, "#%d [%s]%s TRB 0x%02x ", (index+1), v_trb_boards[index].location,
				(v_trb_boards[index].selected)?"[*]":"", v_trb_boards[index].code);
		if (v_trb_boards[index].ready) {
			result++;
			snprintf(status, d_string_buffer_size, "[%sready%s]", v_console_styles[e_console_style_green],
					v_console_styles[e_console_style_reset]);
			if (v_trb_boards[index].stream.stream) {
				value = v_trb_boards[index].stream.written_bytes;
				postfix = f_commands_bytes_extension(&value);
				snprintf(stream, d_string_buffer_size, "{output: %sopen%s}[%.02f %s | %s]\n", v_console_styles[e_console_style_green],
						v_console_styles[e_console_style_reset], value, postfix, v_trb_boards[index].stream.destination);
			} else
				snprintf(stream, d_string_buffer_size, "{output: %sclose%s}\n", v_console_styles[e_console_style_red],
						v_console_styles[e_console_style_reset]);
			strncat(status, stream, (f_string_strlen(status)-d_string_buffer_size));
		} else
			snprintf(status, d_string_buffer_size, "[%soffline%s]\n", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		strncat(buffer, backup, (f_string_strlen(buffer)-d_commands_buffer_size));
		strncat(buffer, status, (f_string_strlen(buffer)-d_commands_buffer_size));
	}
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}

d_define_command(mask) {
	unsigned char mask = 0x00;
	char *string, buffer[d_string_buffer_size];
	int result = d_true, index, status;
	if ((index = d_commands_argument("-m", tokens, elements, NULL, d_console_descriptor_null)) != d_commands_argument_null) {
		string = tokens[index];
		for (index = 0; index < d_trb_boards; ++index) {
			status = d_false;
			if (*string) {
				if ((*string) == '1')
					status = d_true;
				string++;
			}
			if (status)
				mask |= ((unsigned char)(0x01))<<index;
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
	unsigned char packet[d_trb_command_size-1];
	char *string, singleton[(d_commands_hexadecimal_size+1)], buffer[d_string_buffer_size];
	int result = d_false, index, index_hexadecimal;
	if ((index = d_commands_argument("-x", tokens, elements, "parameter '-x' not found\n", output)) != d_commands_argument_null) {
		string = tokens[index];
		singleton[d_commands_hexadecimal_size] = '\0';
		for (index = 0; index < d_trb_command_size; ++index) {
			for (index_hexadecimal = 0; index_hexadecimal < d_commands_hexadecimal_size; ++index_hexadecimal)
				if (*string) {
					singleton[index_hexadecimal] = *string;
					string++;
				} else
					singleton[index_hexadecimal] = 0;
				packet[index] = (unsigned char)strtol(singleton, NULL, 16);
		}
		f_trb_broadcast(packet[0], packet[1], packet[2]);
		snprintf(buffer, d_string_buffer_size, "%d bytes sent (hexadecimal output):\n0x%02x 0x%02x 0x%02x\n", d_trb_command_size, packet[0], packet[1],
				packet[2]);
		if (output != d_console_descriptor_null)
			write(output, buffer, f_string_strlen(buffer));
		result = d_true;
	}
	return result;
}

d_define_command(recv) {
	unsigned char input[d_string_buffer_size];
	char buffer[d_commands_buffer_size] = {0}, backup[d_string_buffer_size], status[d_string_buffer_size];
	int result = d_false, index, index_hexadecimal, timeout = d_trb_common_timeout, readed;
	if ((index = d_commands_argument("-t", tokens, elements, NULL, d_console_descriptor_null)) != d_commands_argument_null)
		timeout = atoi(tokens[index]);
	for (index = 0; index < d_trb_boards; ++index)
		if ((v_trb_boards[index].ready) && (v_trb_boards[index].selected)) {
			if ((readed = f_rs232_read_packet(v_trb_boards[index].descriptor, input, d_string_buffer_size, timeout, v_trb_raw_head,
							v_trb_raw_tail, d_trb_sentinel_size)) > 0) {
				f_trb_check_status(input, readed);
				snprintf(status, d_string_buffer_size, "#%d [%s] %d bytes readed (hexadecimal output):\n", index, v_trb_boards[index].location,
						readed);
				strncat(buffer, status, (d_commands_buffer_size-f_string_strlen(buffer)));
				for (index_hexadecimal = 0; index_hexadecimal < readed; ++index_hexadecimal) {
					snprintf(backup, d_string_buffer_size, "%02x ", input[index_hexadecimal]);
					strncat(buffer, backup, (d_commands_buffer_size-f_string_strlen(buffer)));
				}
				strncat(buffer, "\n", (d_commands_buffer_size-f_string_strlen(buffer)));
			} else {
				snprintf(status, d_string_buffer_size, "#%d [%s] is empty\n", index, v_trb_boards[index].location);
				strncat(buffer, status, (d_commands_buffer_size-f_string_strlen(buffer)));
			}
		}
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}

d_define_command(store) {
	char *string, nomenclature[d_string_buffer_size], buffer[d_string_buffer_size];
	int index, result = d_false;
	if ((index = d_commands_argument("-o", tokens, elements, "parameter '-o' not found", d_console_descriptor_null)) != d_commands_argument_null) {
		string = tokens[index];
		for (index = 0; index < d_trb_boards; ++index)
			if ((v_trb_boards[index].ready) && (v_trb_boards[index].selected)) {
				snprintf(nomenclature, d_string_buffer_size, "%s_TRB%d.bin", string, index);
				f_trb_set_stream(index, nomenclature);
			}
		snprintf(buffer, d_string_buffer_size, "output has been redirected to %s_TRBX.bin\n", string);
		if (output != d_console_descriptor_null)
			write(output, buffer, f_string_strlen(buffer));
	}
	return result;
}

d_define_command(trigger) {
	char buffer[d_string_buffer_size] = {0};
	enum e_adlink_trigger trigger = e_adlink_trigger_50;
	int result = d_false, index, speed;
	if (d_commands_flag("-off", tokens, elements, NULL, d_console_descriptor_null) != d_commands_argument_null)
		trigger = e_adlink_trigger_disabled;
	else if (d_commands_flag("-ext", tokens, elements, NULL, d_console_descriptor_null) != d_commands_argument_null)
		trigger = e_adlink_trigger_external;
	else if ((index = d_commands_argument("-s", tokens, elements, NULL, d_console_descriptor_null)) != d_commands_argument_null)
		switch ((speed = atoi(tokens[index]))) {
			case 100:
				trigger = e_adlink_trigger_100;
				break;
			case 200:
				trigger = e_adlink_trigger_200;
				break;
			case 300:
				trigger = e_adlink_trigger_300;
				break;
			case 50:
			default:
				trigger = e_adlink_trigger_50;
		}
	if ((result = f_adlink_trigger_setup(trigger)))
		switch (trigger) {
			case e_adlink_trigger_disabled:
				strncat(buffer, "trigger has been disabled\n", d_string_buffer_size);
				break;
			case e_adlink_trigger_external:
				strncat(buffer, "running external trigger\n", d_string_buffer_size);
				break;
			case e_adlink_trigger_50:
			case e_adlink_trigger_100:
			case e_adlink_trigger_200:
			case e_adlink_trigger_300:
				snprintf(buffer, d_string_buffer_size, "running trigger @ %dHz\n", trigger);
		}
	else
		snprintf(buffer, d_string_buffer_size, "%sYEK!%s 404 on trigger board\n", v_console_styles[e_console_style_red],
				v_console_styles[e_console_style_reset]);
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}
