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
	4, 	/* board_code 						*/
	5, 	/* command code 					*/
	10,	/* 0x05 						*/
	6, 	/* 0x05 - current @ 3.4V - satellite small packet	*/
	7, 	/* 0x05 - current @ -3.3V - satellite small packet	*/
	8, 	/* 0x05 - current @ 5.7V - satellite small packet	*/
	9, 	/* 0x05 - current @ 12.0V - satellite small packet	*/
	58,	/* 0x06							*/
	54,	/* 0x06 - temperature ADC BOARD				*/
	55,	/* 0x06 - temperature POWER BOARD			*/
	56,	/* 0x06 - temperature FPGA BUS A #1			*/
	57,	/* 0x06 - temperature FPGA BUS B #2			*/
	6,	/* 0x06 - first TFH temperature 			*/
	40,	/* 0x07							*/
	32,	/* 0x07 - current on VSSA1				*/
	33,	/* 0x07 - current on VSSA2				*/
	34,	/* 0x07 - current on VSSA3				*/
	35,	/* 0x07 - current on VSSA4				*/
	36,	/* 0x07 - current on VSSA5				*/
	37,	/* 0x07 - current on VSSA6				*/
	38,	/* 0x07 - current VDD1 @ 3.3V				*/
	39,	/* 0x07 - current VDD2 @ 3.3V				*/
	7,	/* 0x07 - control packet (AA/BB)			*/
	10,	/* 0x07BB - current @ 3.4V				*/
	11,	/* 0x07BB - current @ -3.3V				*/
	12,	/* 0x07BB - current @ 5.7V				*/
	13,	/* 0x07BB - current @ 12V				*/
	8,	/* 0x07BB - BIAS voltage #1				*/
	9,	/* 0x07BB - BIAS voltage #2				*/
	11,	/* 0x07AA - hold delay value				*/
	14,	/* 0x07 - trigger low byte				*/
	15,	/* 0x07 - trigger high byte				*/
	29,	/* 0x07 - version code A				*/
	31	/* 0x07 - version code B				*/
};
struct s_trb_device v_trb_device_boards[d_trb_device_boards] = {
	{d_rs232_null, d_false, d_false, d_false, 0x00, "/dev/ttyL0"},
	{d_rs232_null, d_false, d_false, d_false, 0x01, "/dev/ttyL1"},
	{d_rs232_null, d_false, d_false, d_false, 0x08, "/dev/ttyL2"},
	{d_rs232_null, d_false, d_false, d_false, 0x09, "/dev/ttyL3"},
	{d_rs232_null, d_false, d_false, d_false, 0x04, "/dev/ttyL4"},
	{d_rs232_null, d_false, d_false, d_false, 0x05, "/dev/ttyL5"},
	{d_rs232_null, d_false, d_false, d_false, 0x0C, "/dev/ttyL6"},
	{d_rs232_null, d_false, d_false, d_false, 0x0D, "/dev/ttyL7"}
};
void p_trb_device_description_format(unsigned char code, char *destination, size_t size) {
	static const char *bytes_postfixes[] = {
		"B",
		"kB",
		"MB",
		"GB",
		"TB",
		"YB",
		NULL
	};
	char status[d_string_buffer_size], stream[d_string_buffer_size];
	float value;
	int postfix;
	snprintf(destination, size, "#%d [%s]%s TRB 0x%02x ", code, v_trb_device_boards[code].location, (v_trb_device_boards[code].selected)?"[*]":"",
			v_trb_device_boards[code].code);
	if (f_trb_device_initialize(code)) {
		if (v_trb_device_boards[code].wrong)
			snprintf(status, d_string_buffer_size, "[%swrong connector%s]", v_console_styles[e_console_style_yellow],
					v_console_styles[e_console_style_reset]);
		else
			snprintf(status, d_string_buffer_size, "[%sready%s]", v_console_styles[e_console_style_green], v_console_styles[e_console_style_reset]);
		if (v_trb_device_boards[code].stream.stream) {
			value = v_trb_device_boards[code].stream.written_bytes;
			for (postfix = 0; (bytes_postfixes[postfix+1]) && (value >= 1024.0); ++postfix)
				value /= 1024.0;
			snprintf(stream, d_string_buffer_size, "{%sW%s}[%4.02f %s| %s]", v_console_styles[e_console_style_green],
					v_console_styles[e_console_style_reset], value, bytes_postfixes[postfix], v_trb_device_boards[code].stream.destination);
		} else
			snprintf(stream, d_string_buffer_size, "{%sR%s}", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		strncat(status, stream, (d_string_buffer_size-f_string_strlen(status)));
	} else
		snprintf(status, d_string_buffer_size, "[%soffline%s]", v_console_styles[e_console_style_red], v_console_styles[e_console_style_reset]);
	strncat(destination, status, (size-f_string_strlen(destination)));
}

int f_trb_device_description(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_console_output_size];
	int result = d_true;
	if (output != d_console_descriptor_null) {
		p_trb_device_description_format(code, buffer, d_console_output_size);
		write(output, buffer, f_string_strlen(buffer));
		write(output, "\n", f_string_strlen("\n"));
		fsync(output);
	}
	return result;
}

void p_trb_device_status_dump(unsigned char code, const char *path) {
	FILE *stream;
	int index;
	if ((stream = fopen(path, "a"))) {
		fprintf(stream, "%ld%c0x%02x", v_trb_device_boards[code].last_refresh, d_trb_device_csv_character, code);
		for (index = 0; index < e_trb_device_currents_null; ++index)
			fprintf(stream, "%c%.02f", d_trb_device_csv_character, V(code).currents[index]);
		for (index = 0; index < e_trb_device_temperatures_null; ++index)
			fprintf(stream, "%c%.02f", d_trb_device_csv_character, V(code).temperatures[index]);
		for (index = 0; index < d_trb_device_temperatures_size; ++index)
			fprintf(stream, "%c%.02f", d_trb_device_csv_character, V(code).tfh_temperatures[index]);
		for (index = 0; index < e_trb_device_voltages_null; ++index)
			fprintf(stream, "%c%.02f", d_trb_device_csv_character, V(code).voltages[index]);
		for (index = 0; index < e_trb_device_status_null; ++index)
			fprintf(stream, "%c0x%02x", d_trb_device_csv_character, V(code).status[index]);
		fputc('\n', stream);
		fclose(stream);
	}
}

int p_trb_device_status_refresh(unsigned char code) {
	static unsigned char status_requests[] = {0x06, 0x07};
	unsigned char raw_command[d_trb_device_raw_command_size];
	int index, result = d_true;
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		v_trb_device_boards[code].last_refresh = time(NULL);
		/* refresh the status of our TRB (sorry but I have to hardcode this shi**ing incoherent stuff) */
		for (index = 0; index < sizeof(status_requests); ++index) {
			p_trb_device_write_packet(raw_command, v_trb_device_boards[code].code, status_requests[index], 0x00, 0x00);
			if (f_rs232_write(v_trb_device_boards[code].descriptor, raw_command, d_trb_device_raw_command_size) == d_trb_device_raw_command_size)
				p_trb_device_refresh_status(code);
			else {
				result = d_false;
				break;
			}
		}
		if (result)
			p_trb_device_status_dump(code, d_trb_device_log);
	}
	return result;
}

int f_trb_device_status(unsigned char code, char **tokens, size_t elements, int output) {
	char currents[d_string_buffer_size], temperatures[d_string_buffer_size], voltages[d_string_buffer_size], status[d_string_buffer_size];
	int argument, selected = d_true, result = d_true;
	if ((argument = f_console_parameter("-d", tokens, elements, d_false)) != d_console_descriptor_null)
		if (code != atoi(tokens[argument]))
			selected = d_false;
	if (selected) {
		p_trb_device_status_refresh(code);
		if ((result = f_trb_device_description(code, tokens, elements, output))) {
			if (output != d_console_descriptor_null) {
				snprintf(currents, d_string_buffer_size, "%scurrents%s\n\t[+3.4V %4.02fmA]\n\t[-3.3V %4.02fmA]\n"
						"\t[+5.7V %4.02fmA]\n\t[+12V  % 4.02fmA]\n\t[VSSA1 %4.02fmA|%4.02fmA VSSA2]\n"
						"\t[VSSA3 %4.02fmA|%4.02fmA VSSA4]\n\t[VSSA5 %4.02fmA|%4.02fmA VSSA6]\n"
						"\t[VDD1  %4.02fmA|%4.02fmA  VDD2]\n",
						v_console_styles[e_console_style_yellow], v_console_styles[e_console_style_reset],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_34],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_57],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_12],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA1],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA2],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA3],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA4],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA5],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA6],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33VDD1],
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33VDD2]);
				write(output, currents, f_string_strlen(currents));
				snprintf(temperatures, d_string_buffer_size, "%stemperatures%s\n\t[ADC    %4.02fC | %4.02fC    PWR]\n"
						"\t[FPGA A %4.02fC | %4.02fC FPGA B]\n",
						v_console_styles[e_console_style_yellow], v_console_styles[e_console_style_reset],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_adc],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_power],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_fpga_A],
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_fpga_B]);
				write(output, temperatures, f_string_strlen(temperatures));
				snprintf(voltages, d_string_buffer_size, "%svoltages%s\n\t[HV#1 %s%4.02f%sV]\n\t[HV#2 %s%4.02f%sV]\n",
						v_console_styles[e_console_style_yellow], v_console_styles[e_console_style_reset],
						v_console_styles[e_console_style_bold], v_trb_device_boards[code].status.voltages[e_trb_device_voltages_HV1],
						v_console_styles[e_console_style_reset], v_console_styles[e_console_style_bold],
						v_trb_device_boards[code].status.voltages[e_trb_device_voltages_HV2], v_console_styles[e_console_style_reset]);
				write(output, voltages, f_string_strlen(voltages));
				snprintf(status, d_string_buffer_size, "%sstatus%s\n\t[Trigger   : %5d]\n\t[Hold Delay: %4.01fuS]\n",
						v_console_styles[e_console_style_yellow], v_console_styles[e_console_style_reset],
						v_trb_device_boards[code].trigger,
						((float)((unsigned int)v_trb_device_boards[code].status.status[e_trb_device_status_HD]*50)/1000.0));
				write(output, status, f_string_strlen(status));
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

int f_trb_device_focus(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	int argument, selected = d_true, result = d_true;
	if ((argument = f_console_parameter("-d", tokens, elements, d_false)) != d_console_descriptor_null)
		if (code != atoi(tokens[argument]))
			selected = d_false;
	v_trb_device_boards[code].focused = selected;
	if (output != d_console_descriptor_null)
		if (v_trb_device_boards[code].focused) {
			snprintf(buffer, d_string_buffer_size, "magrathea's focus is now on TRB #%d\n", code);
			write(output, buffer, f_string_strlen(buffer));
			fsync(output);
		}
	return result;
}

int f_trb_device_view(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	int argument, result = d_true, ladder = 0;
	FILE *process;
	if ((argument = f_console_parameter("-l", tokens, elements, d_false)) != d_console_descriptor_null)
		ladder = atoi(tokens[argument]);
	if (v_trb_device_boards[code].stream.stream) {
		if (output != d_console_descriptor_null)
			if (v_trb_device_boards[code].focused) {
				snprintf(buffer, d_string_buffer_size, "magrathea's view is now on TRB #%d @ ladder %d\n", code, ladder);
				write(output, buffer, f_string_strlen(buffer));
				fsync(output);
			}
		snprintf(buffer, d_string_buffer_size, "%s %s %d %d", d_trb_device_viewer, v_trb_device_boards[code].stream.destination, ladder,
				d_trb_device_viewer_skip);
		if ((process = popen(buffer, "r")))
			fclose(process);
		snprintf(buffer, d_string_buffer_size, "%s %s 0x%02x", d_trb_device_status, d_trb_device_log, v_trb_device_boards[code].code);
		if ((process = popen(buffer, "r")))
			fclose(process);
	}
	return result;
}

int f_trb_device_convert(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	int result = d_true;
	FILE *process;
	if (v_trb_device_boards[code].stream.stream) {
		if (output != d_console_descriptor_null)
			if (v_trb_device_boards[code].focused) {
				snprintf(buffer, d_string_buffer_size, "magrathea is converting now data from TRB #%d\n", code);
				write(output, buffer, f_string_strlen(buffer));
				fsync(output);
			}
		snprintf(buffer, d_string_buffer_size, "%s %s", d_trb_device_converter, v_trb_device_boards[code].stream.destination);
		if ((process = popen(buffer, "r")))
			fclose(process);
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
						v_trb_device_raw_head, v_trb_device_raw_tail, d_trb_device_sentinel_size)) > 0) {
			v_trb_device_boards[code].wrong = d_false;
			if (v_trb_device_boards[code].code != loopback_answer[4])
				v_trb_device_boards[code].wrong = d_true;
			result = d_true;
		} else
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
	int index;
	float value, current, temperature, voltage;
	if (size > B(e_trb_device_bytes_board_code))
		if (v_trb_device_boards[code].code == buffer[B(e_trb_device_bytes_board_code)]) {
			/* I'm so sorry for this bunch of sh*t but fine tuning is needed */
			switch (buffer[B(e_trb_device_bytes_command)]) {
				case 0x05:
					if (size > B(e_trb_device_bytes_0x05)) {
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_34)]);
						current = (value*8.0f)-19.6f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_34] = (value==0)?value:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_33)]);
						current = (value*8.0f)-7.1f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_57)]);
						current = (value*0.8f)-5.1f;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_57] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x05_current_12)]);
						current = value;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_12] = (current<0.0f)?0.0f:current;
					}
					break;
				case 0x06:
					if (size > B(e_trb_device_bytes_0x06)) {
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_adc)])*0.008;
						temperature = (-106.7*value*value*value)+(244.4*value*value)-(238.6*value)+108.7;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_adc] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_power)])*0.008;
						temperature = (-106.7*value*value*value)+(244.4*value*value)-(238.6*value)+108.7;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_power] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_fpga_A)])*0.008;
						temperature = (-106.7*value*value*value)+(244.4*value*value)-(238.6*value)+108.7;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_fpga_A] = temperature;
						value = (buffer[B(e_trb_device_bytes_0x06_temperature_fpga_B)])*0.008;
						temperature = (-106.7*value*value*value)+(244.4*value*value)-(238.6*value)+108.7;
						v_trb_device_boards[code].status.temperatures[e_trb_device_temperatures_fpga_B] = temperature;
						for (index = 0; index < d_trb_device_temperatures_size; ++index)
							v_trb_device_boards[code].status.tfh_temperatures[index] =
								buffer[index+B(e_trb_device_bytes_0x06_temperature_TFH)];
					}
					break;
				case 0x07:
					if (size > B(e_trb_device_bytes_0x07)) {
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA1)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA1] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA2)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA2] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA3)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA3] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA4)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA4] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA5)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA5] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_VSSA6)]);
						current = (value*1.7422)-2.5795;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_VSSA6] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_33VDD1)]);
						current = value*0.8;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33VDD1] = (current<0.0f)?0.0f:current;
						value = (int)(buffer[B(e_trb_device_bytes_0x07_current_33VDD2)]);
						current = value*0.8;
						v_trb_device_boards[code].status.currents[e_trb_device_currents_33VDD2] = (current<0.0f)?0.0f:current;
						if (buffer[B(e_trb_device_bytes_0x07_control)] == 0xAA) {
							v_trb_device_boards[code].status.status[e_trb_device_status_HD] =
								buffer[B(e_trb_device_bytes_0x07AA_status_HD)];
						} else if (buffer[B(e_trb_device_bytes_0x07_control)] == 0xBB) {
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_current_S34)]);
							current = (value*8.0f)-19.6f;
							v_trb_device_boards[code].status.currents[e_trb_device_currents_34] = (value==0)?value:current;
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_current_S33)]);
							current = (value*8.0f)-7.1f;
							v_trb_device_boards[code].status.currents[e_trb_device_currents_33] = (current<0.0f)?0.0f:current;
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_current_S57)]);
							current = (value*0.8f)-5.1f;
							v_trb_device_boards[code].status.currents[e_trb_device_currents_57] = (current<0.0f)?0.0f:current;
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_current_S12)]);
							current = value;
							v_trb_device_boards[code].status.currents[e_trb_device_currents_12] = (current<0.0f)?0.0f:current;
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_voltage_HV1)]);
							voltage = (value*0.7246)-21.6207;
							v_trb_device_boards[code].status.voltages[e_trb_device_voltages_HV1] = (value<50)?0.0f:voltage;
							value = (int)(buffer[B(e_trb_device_bytes_0x07BB_voltage_HV2)]);
							voltage = (value*0.7246)-21.6207;
							v_trb_device_boards[code].status.voltages[e_trb_device_voltages_HV2] = (value<50)?0.0f:voltage;
						}
						v_trb_device_boards[code].status.status[e_trb_device_status_trigger_low] =
							buffer[B(e_trb_device_bytes_0x07_status_trigger_low)];
						v_trb_device_boards[code].status.status[e_trb_device_status_trigger_high] =
							buffer[B(e_trb_device_bytes_0x07_status_trigger_high)];
						v_trb_device_boards[code].trigger =
							(((unsigned int)buffer[B(e_trb_device_bytes_0x07_status_trigger_low)])&0xFF)|
							(((unsigned int)buffer[B(e_trb_device_bytes_0x07_status_trigger_high)])<<8);
						v_trb_device_boards[code].status.status[e_trb_device_status_version_M] =
							buffer[B(e_trb_device_bytes_0x07_status_version_M)];
						v_trb_device_boards[code].status.status[e_trb_device_status_version_L] =
							buffer[B(e_trb_device_bytes_0x07_status_version_L)];
					}
					break;
			}
		}
}

int p_trb_device_refresh_status(unsigned char code) {
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

int f_trb_device_refresh(unsigned char code, struct s_console *console) {
	int result = p_trb_device_refresh_status(code);
	char buffer[d_string_buffer_size];
	time_t current_time;
	if (v_trb_device_boards[code].descriptor != d_rs232_null) {
		current_time = time(NULL);
		if ((v_trb_device_boards[code].last_refresh+d_trb_device_timeout_status) < current_time)
			p_trb_device_status_refresh(code);
		if (console)
			if (v_trb_device_boards[code].focused) {
				p_trb_device_description_format(code, buffer, d_string_buffer_size);
				snprintf(console->prefix, d_string_buffer_size, "\r[%s]>", buffer);
			}
	}
	return result;
}
