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
#include "trb_device.h"
unsigned char v_trb_device_raw_head[] = {0x55, 0xaa}, v_trb_device_raw_tail[] = {0x5a, 0xa5};
unsigned int v_trb_device_bytes[] = {
	4, 	/* board_code 			*/
	5, 	/* command code 		*/
	10,	/* 0x05 			*/
	6, 	/* 0x05 - current @ 3.4V	*/
	7, 	/* 0x05 - current @ -3.3V	*/
	8, 	/* 0x05 - current @ 5.7V	*/
	9, 	/* 0x05 - current @ 12.0V	*/
	58,	/* 0x06				*/
	54,	/* 0x06 - temperature BUS A #1	*/
	55,	/* 0x06 - temperature BUS B #1	*/
	56,	/* 0x06 - temperature BUS A #2	*/
	57	/* 0x06 - temperature BUS B #3	*/
};
struct s_trb_device v_trb_device_boards[d_trb_device_boards] = {
	{d_rs232_null, d_false, 0x00, "/dev/ttyL0"},
	{d_rs232_null, d_false, 0x01, "/dev/ttyL3"},
	{d_rs232_null, d_false, 0x08, "/dev/ttyL1"},
	{d_rs232_null, d_false, 0x09, "/dev/ttyL2"},
	{d_rs232_null, d_false, 0x04, "/dev/ttyL4"},
	{d_rs232_null, d_false, 0x05, "/dev/ttyL7"},
	{d_rs232_null, d_false, 0x0C, "/dev/ttyL5"},
	{d_rs232_null, d_false, 0x0D, "/dev/ttyL6"}
};
int f_trb_device_description(unsigned char code, char **tokens, size_t elements, int output) {
	static const char *bytes_postfixes[] = {
		"B",
		"kB",
		"MB",
		"GB",
		"TB",
		"YB",
		NULL
	};
	char buffer[d_console_output_size] = {0}, status[d_string_buffer_size], stream[d_string_buffer_size];
	float value;
	int result = d_true, postfix;
	if (output != d_console_descriptor_null) {
		snprintf(buffer, d_console_output_size, "#%d [%s]%s TRB 0x%02x ", code, v_trb_device_boards[code].location,
				(v_trb_device_boards[code].selected)?"[*]":"", v_trb_device_boards[code].code);
		if (f_trb_device_initialize(code)) {
			snprintf(status, d_string_buffer_size, "[%sready%s]", v_console_styles[e_console_style_green],
					v_console_styles[e_console_style_reset]);
			if (v_trb_device_boards[code].stream.stream) {
				value = v_trb_device_boards[code].stream.written_bytes;
				for (postfix = 0; (bytes_postfixes[postfix+1]) && (value >= 1024.0); ++postfix)
					value /= 1024.0;
				snprintf(stream, d_string_buffer_size, "{output: %sopen%s}[%.02f %s| %s]\n", v_console_styles[e_console_style_green],
						v_console_styles[e_console_style_reset], value, bytes_postfixes[postfix],
						v_trb_device_boards[code].stream.destination);
			} else
				snprintf(stream, d_string_buffer_size, "{output: %sclose%s}\n", v_console_styles[e_console_style_red],
						v_console_styles[e_console_style_reset]);
			strncat(status, stream, (d_string_buffer_size-f_string_strlen(status)));
		} else
			snprintf(status, d_string_buffer_size, "[%soffline%s]\n", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		strncat(buffer, status, (d_console_output_size-f_string_strlen(buffer)));
	}
	if (output != d_console_descriptor_null)
		write(output, buffer, f_string_strlen(buffer));
	return result;
}

int f_trb_device_status(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_console_output_size] = {0}, currents[d_string_buffer_size], temperatures[d_string_buffer_size];
	int argument, selected = d_true, result = d_true;
	if ((argument = f_console_parameter("-d", tokens, elements, d_false)) != d_console_descriptor_null)
		if (code != atoi(tokens[argument]))
			selected = d_false;
	if (selected) {
		if ((result = f_trb_device_description(code, tokens, elements, output))) {
			if (output != d_console_descriptor_null) {
				snprintf(currents, d_string_buffer_size, "[+3.4V %4.02fmA | -3.3V %4.02fmA | +5.7V %4.02fmA | +12V %4.02fmA]\n",
						v_trb_device_boards[code].status.currents[e_trb_device_currents_34],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_57],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_12]);
				snprintf(temperatures, d_string_buffer_size, "[A %5.02fC - %5.02fC | %5.02fC - %5.02fC B]\n",
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSA_1],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSB_1],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSA_2],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSB_2]);
				strncat(buffer, currents, (d_console_output_size-f_string_strlen(buffer)));
				strncat(buffer, temperatures, (d_console_output_size-f_string_strlen(buffer)));
				write(output, buffer, f_string_strlen(buffer));
				fsync(output);
			}
		} else
			result = d_false;
	}
	return result;
}

int f_trb_device_stream(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	int result = d_true;
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		p_trb_device_destroy_stream(code);
		snprintf(buffer, d_string_buffer_size, "%s_TRB%d.bin", tokens[f_console_parameter("-o", tokens, elements, d_false)], code);
		if ((v_trb_device_boards[code].stream.stream = fopen(buffer, "w")))
			strcpy(v_trb_device_boards[code].stream.destination, buffer); /* same length */
	}
	return result;
}

void p_trb_device_write_packet(unsigned char *supplied, unsigned char code, unsigned char type, unsigned char high_byte, unsigned char low_byte) {
	static unsigned char raw_command[d_trb_device_raw_command_size] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned int checksum = (code+type+high_byte+low_byte);
	memcpy(supplied, raw_command, d_trb_device_raw_command_size);
	supplied[4] = code;
	supplied[5] = type;
	supplied[6] = high_byte;
	supplied[7] = low_byte;
	supplied[8] = (checksum>>8)&0xff;
	supplied[9] = (checksum)&0xff;
}

int f_trb_device_write(unsigned char code, char **tokens, size_t elements, int output) {
	unsigned char packet[(d_trb_device_command_size-1)], raw_command[d_trb_device_raw_command_size];
	char *string = tokens[f_console_parameter("-x", tokens, elements, d_false)], singleton[(d_trb_device_hexadecimal_size+1)], buffer[d_string_buffer_size];
	int index, hexadecimal, result = d_true;
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		for (index = 0; index < (d_trb_device_command_size-1); ++index) {
			memset(singleton, 0, (d_trb_device_hexadecimal_size+1));
			for (hexadecimal = 0; hexadecimal < d_trb_device_hexadecimal_size; ++hexadecimal) {
				if (*string) {
					singleton[hexadecimal] = *string;
					string++;
				} else
					singleton[hexadecimal] = 0;
			}
			packet[index] = (unsigned char)strtol(singleton, NULL, 16);
		}
		p_trb_device_write_packet(raw_command, v_trb_device_boards[code].code, packet[0], packet[1], packet[2]);
		if (!(result = (f_rs232_write(v_trb_device_boards[code].descriptor, raw_command,
							d_trb_device_raw_command_size) == d_trb_device_raw_command_size)))
			f_trb_device_destroy(code);
		if (output != d_console_descriptor_null) {
			if (v_trb_device_boards[code].descriptor != d_rs232_null)
				snprintf(buffer, d_string_buffer_size, "#%d TRB: %d bytes sent (hexadecimal output): 0x%02x 0x%02x 0x%02x 0x%02x\n",
						code, d_trb_device_command_size, v_trb_device_boards[code].code, packet[0], packet[1], packet[2]);
			else
				snprintf(buffer, d_string_buffer_size, "#%d TRB has been %sclosed%s due to a I/O error\n", code,
						v_console_styles[e_console_style_red], v_console_styles[e_console_style_reset]);
			write(output, buffer, f_string_strlen(buffer));
			fsync(output);
		}
	}
	return result;
}

int f_trb_device_mask(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	int argument, selected = d_false, result = d_true;
	if (f_console_parameter("-a", tokens, elements, d_true) != d_console_parameter_null)
		selected = d_true;
	else if ((argument = f_console_parameter("-m", tokens, elements, d_false)) != d_console_descriptor_null)
		if (f_string_strlen(tokens[argument]) > code)
			if (tokens[argument][code] == '1')
				selected = d_true;
	v_trb_device_boards[code].selected = selected;
	if (output != d_console_descriptor_null) {
		if (v_trb_device_boards[code].selected)
			snprintf(buffer, d_string_buffer_size, "#%d TRB has been %sselected%s\n", code, v_console_styles[e_console_style_green],
					v_console_styles[e_console_style_reset]);
		else
			snprintf(buffer, d_string_buffer_size, "#%d TRB has been %sdeselected%s\n", code, v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		write(output, buffer, f_string_strlen(buffer));
		fsync(output);
	}
	return result;
}

int f_trb_device_enabled(unsigned char code) {
	return v_trb_device_boards[code].selected;
}

int f_trb_device_initialize(unsigned char code) {
	static unsigned char bus_loopback[] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned char loopback_answer[d_trb_device_raw_command_size];
	int readed, result = d_false;
	if (v_trb_device_boards[code].descriptor == d_rs232_null)
		if (!f_rs232_open(v_trb_device_boards[code].location, e_rs232_baud_115200, e_rs232_bits_8, e_rs232_stops_2_bit, e_rs232_parity_odd,
					e_rs232_flow_control_no, &(v_trb_device_boards[code].descriptor), &(v_trb_device_boards[code].old_configuration)))
			v_trb_device_boards[code].descriptor = d_rs232_null;
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		f_rs232_write(v_trb_device_boards[code].descriptor, bus_loopback, d_trb_device_raw_command_size);
		if ((readed = f_rs232_read_packet(v_trb_device_boards[code].descriptor, loopback_answer, d_trb_device_raw_command_size, d_trb_device_timeout,
					v_trb_device_raw_head, v_trb_device_raw_tail, d_trb_device_sentinel_size)) > 0)
			result = d_true;
		else
			f_trb_device_destroy(code);
	}
	return result;
}

void p_trb_device_destroy_descriptor(unsigned char code) {
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		f_rs232_close_termios(v_trb_device_boards[code].descriptor, v_trb_device_boards[code].old_configuration);
		v_trb_device_boards[code].descriptor = d_rs232_null;
	}
}

void p_trb_device_destroy_stream(unsigned char code) {
	if (v_trb_device_boards[code].stream.stream) {
		fclose(v_trb_device_boards[code].stream.stream);
		v_trb_device_boards[code].stream.stream = NULL;
	}
	v_trb_device_boards[code].stream.written_bytes = 0;
}

int f_trb_device_destroy(unsigned char code) {
	int result = d_true;
	p_trb_device_destroy_descriptor(code);
	p_trb_device_destroy_stream(code);
	return result;
}

void p_trb_device_refresh_analyze(unsigned char code, unsigned char *buffer, size_t size) {
	float value, current, temperature;
	if (size > B(e_trb_device_bytes_board_code))
		if (v_trb_device_boards[code].code == buffer[B(e_trb_device_bytes_board_code)]) {
			switch (buffer[B(e_trb_device_bytes_command)]) {
				case 0x05:
					if (size > B(e_trb_device_bytes_0x05)) {
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_34)]);
						current = (value*8.0f)+25.0f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_34] = (value==0)?value:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_33)]);
						current = (value*8.0f);
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_57)]);
						current = (value*0.8f)-4.0f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_57] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_12)]);
						current = (value*0.8f)-2.7f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_12] = (current<0.0f)?0.0f:current;
					}
					break;
				case 0x06:
					if (size > B(e_trb_device_bytes_0x06)) {
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_A1)])*0.008;
						temperature = (-136.64*value*value*value)+(304.47*value*value)-(285.48*value)+139.21;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSA_1] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_B1)])*0.008;
						temperature = (-136.64*value*value*value)+(304.47*value*value)-(285.48*value)+139.21;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSB_1] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_A2)])*0.008;
						temperature = (-136.64*value*value*value)+(304.47*value*value)-(285.48*value)+139.21;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSA_2] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_B2)])*0.008;
						temperature = (-136.64*value*value*value)+(304.47*value*value)-(285.48*value)+139.21;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_BUSB_2] = temperature;
					}
					break;
				case 0x07:
					break;
			}
		}
}

int f_trb_device_refresh(unsigned char code) {
	unsigned char raw_answer[d_trb_device_raw_answer_size];
	int readed = d_true, result = d_true;
	while ((readed > 0) && (v_trb_device_boards[code].descriptor != d_rs232_null))
		if ((readed = f_rs232_read_packet(v_trb_device_boards[code].descriptor, raw_answer, d_trb_device_raw_answer_size,
						d_trb_device_timeout, v_trb_device_raw_head, v_trb_device_raw_tail, d_trb_device_sentinel_size)) > 0)
			p_trb_device_refresh_analyze(code, raw_answer, readed);
		else if (readed < 0) {
			f_trb_device_destroy(code);
			result = d_false;
			return result;
		}
	return result;
}
