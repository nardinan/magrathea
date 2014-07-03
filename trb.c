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
#include "trb.h"
struct s_trb v_trb_boards[d_trb_boards] = {
	{d_rs232_null, d_false, d_false, 0x00, "/dev/ttyL0"},
	{d_rs232_null, d_false, d_false, 0x01, "/dev/ttyL3"},
	{d_rs232_null, d_false, d_false, 0x08, "/dev/ttyL1"},
	{d_rs232_null, d_false, d_false, 0x09, "/dev/ttyL2"},
	{d_rs232_null, d_false, d_false, 0x04, "/dev/ttyL4"},
	{d_rs232_null, d_false, d_false, 0x05, "/dev/ttyL7"},
	{d_rs232_null, d_false, d_false, 0x0C, "/dev/ttyL5"},
	{d_rs232_null, d_false, d_false, 0x0D, "/dev/ttyL6"}
};
unsigned int v_trb_bytes[] = {
	4, 	/* board_code 			*/
	5, 	/* command code 		*/
	10,	/* 0x05 			*/
	6, 	/* 0x05 - current @ 3.4V	*/
	7, 	/* 0x05 - current @ -3.3V	*/
	8, 	/* 0x05 - current @ 5.7V	*/
	9  	/* 0x05 - current @ 12.0V	*/
};
unsigned char v_trb_raw_head[d_trb_sentinel_size] = {0x55, 0xaa}, v_trb_raw_tail[d_trb_sentinel_size] = {0x5a, 0xa5};
void f_trb_set_mask(unsigned char flags) {
	int index;
	for (index = 0; index < d_trb_boards; ++index, flags >> 1)
		if (v_trb_boards[index].ready)
			v_trb_boards[index].selected = ((flags&0x1));
}

void f_trb_set_packet(unsigned char *supplied, unsigned char trb, unsigned char type, unsigned char high_byte, unsigned char low_byte) {
	static unsigned char raw_command[d_trb_raw_command_size] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned int checksum = (trb+type+high_byte+low_byte);
	memcpy(supplied, raw_command, d_trb_raw_command_size);
	supplied[4] = trb;
	supplied[5] = type;
	supplied[6] = high_byte;
	supplied[7] = low_byte;
	supplied[8] = (checksum>>8)&0xff;
	supplied[9] = (checksum)&0xff;
}

int f_trb_set_stream(int trb, const char *destination) {
	int result = d_true;
	if (v_trb_boards[trb].stream.stream) {
		fclose(v_trb_boards[trb].stream.stream);
		v_trb_boards[trb].stream.stream = NULL;
	}
	v_trb_boards[trb].stream.written_bytes = 0;
	if (destination) {
		if ((result = ((v_trb_boards[trb].stream.stream = fopen(destination, "w")) != NULL)))
			strncpy(v_trb_boards[trb].stream.destination, destination, d_string_buffer_size);
	} else
		memset(v_trb_boards[trb].stream.destination, 0, d_string_buffer_size);
	return result;
}

void p_trb_output_interpreter(int trb, unsigned char command, unsigned char *buffer, size_t size) {
	int value;
	float current;
	switch (buffer[B(e_trb_bytes_command)]) {
		case 0x05:
			if (size > e_trb_bytes_0x05) {
				value = (int)(buffer[B(e_trb_bytes_0x05_current_34)]);
				current = (value*8.0f)+25.0f;
				v_trb_boards[trb].status.currents[e_trb_currents_34] = (value==0)?value:current;
				value = (int)(buffer[B(e_trb_bytes_0x05_current_33)]);
				current = (value*8.0f);
				v_trb_boards[trb].status.currents[e_trb_currents_33] = (current<0.0f)?0.0f:current;
				value = (int)(buffer[B(e_trb_bytes_0x05_current_57)]);
				current = (value*0.8f)-4.0f;
				v_trb_boards[trb].status.currents[e_trb_currents_57] = (current<0.0f)?0.0f:current;
				value = (int)(buffer[B(e_trb_bytes_0x05_current_12)]);
				current = (value*0.8f)-2.7f;
				v_trb_boards[trb].status.currents[e_trb_currents_12] = (current<0.0f)?0.0f:current;
			}
			break;
		case 0x06:
			break;
		case 0x07:
			break;
	}
}

void f_trb_output_interpreter(unsigned char *buffer, size_t size) {
	int trb;
	if (size > B(e_trb_bytes_board_code)) {
		for (trb = 0; trb < d_trb_boards; trb++)
			if (v_trb_boards[trb].code == buffer[B(e_trb_bytes_board_code)])
		p_trb_output_interpreter(trb, buffer[B(e_trb_bytes_command)], buffer, size);
	}
}

void f_trb_connect(int trb, time_t timeout) {
	static unsigned char bus_loopback[d_trb_raw_command_size] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned char loopback_reply[d_trb_raw_command_size];
	if (!v_trb_boards[trb].ready)
		v_trb_boards[trb].ready = f_rs232_open(v_trb_boards[trb].location, e_rs232_baud_115200, e_rs232_bits_8, e_rs232_stops_2_bit,
				e_rs232_parity_odd, e_rs232_flow_control_no, &(v_trb_boards[trb].descriptor),
				&(v_trb_boards[trb].old_configuration));
	if (v_trb_boards[trb].ready) {
		f_rs232_write(v_trb_boards[trb].descriptor, bus_loopback, d_trb_raw_command_size);
		if (f_rs232_read_packet(v_trb_boards[trb].descriptor, loopback_reply, d_trb_raw_command_size, timeout,
					v_trb_raw_head, v_trb_raw_tail, d_trb_sentinel_size) <= 0)
			f_trb_disconnect(trb);
	}
}

void f_trb_disconnect(int trb) {
	if (v_trb_boards[trb].ready) {
		f_rs232_close_termios(v_trb_boards[trb].descriptor, v_trb_boards[trb].old_configuration);
		v_trb_boards[trb].descriptor = d_rs232_null;
		v_trb_boards[trb].ready = d_false;
	}
	f_trb_set_stream(trb, NULL);
}

void f_trb_broadcast(unsigned char type, unsigned char high_byte, unsigned char low_byte) {
	unsigned char raw_command[d_trb_raw_command_size];
	int index;
	for (index = 0; index < d_trb_boards; ++index)
		if ((v_trb_boards[index].ready) && (v_trb_boards[index].selected)) {
			f_trb_set_packet(raw_command, v_trb_boards[index].code, type, high_byte, low_byte);
			if (f_rs232_write(v_trb_boards[index].descriptor, raw_command, d_trb_raw_command_size) != d_trb_raw_command_size)
				f_trb_disconnect(index);
		}
}

void f_trb_acquire(time_t timeout) {
	int index;
	size_t readed;
	for (index = 0; index < d_trb_boards; index++)
		if ((v_trb_boards[index].ready) && (f_adlink_data_read(index, &readed, v_trb_boards[index].stream.stream, timeout)))
			if (v_trb_boards[index].stream.stream)
				v_trb_boards[index].stream.written_bytes += readed;
}

