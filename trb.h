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
#ifndef magrathea_trb_h
#define magrathea_trb_h
#include "rs232.h"
#include "adlink.h"
#define d_trb_common_timeout 50000
#define d_trb_read_timeout 1000
#define d_trb_command_size 4
#define d_trb_raw_command_size 12
#define d_trb_sentinel_size 2
#define d_trb_boards 8
#define B(a) v_trb_bytes[a]
typedef enum e_trb_currents {
	e_trb_currents_34 = 0,
	e_trb_currents_33,
	e_trb_currents_57,
	e_trb_currents_12,
	e_trb_currents_null
} e_trb_currents;
typedef enum e_trb_bytes {
	e_trb_bytes_board_code = 0,
	e_trb_bytes_command,
	e_trb_bytes_0x05,
	e_trb_bytes_0x05_current_34,
	e_trb_bytes_0x05_current_33,
	e_trb_bytes_0x05_current_57,
	e_trb_bytes_0x05_current_12
} e_trb_bytes;
typedef struct s_trb {
	int descriptor, ready:1, selected:1;
	unsigned char code;
	const char *location;
	struct termios old_configuration;
	struct s_trb_stream {
		char destination[d_string_buffer_size];
		FILE *stream;
		size_t written_bytes;
	} stream;
	struct s_trb_status {
		float currents[e_trb_currents_null];
	} status;

} s_trb;
extern struct s_trb v_trb_boards[d_trb_boards];
extern unsigned int v_trb_bytes[];
extern unsigned char v_trb_raw_head[d_trb_sentinel_size], v_trb_raw_tail[d_trb_sentinel_size];
extern void f_trb_set_mask(unsigned char flag);
extern void f_trb_set_packet(unsigned char *supplied, unsigned char trb, unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern int f_trb_set_stream(int trb, const char *destination);
extern void p_trb_output_interpreter(int trb, unsigned char command, unsigned char *buffer, size_t size);
extern void f_trb_output_interpreter(unsigned char *buffer, size_t size);
extern void f_trb_connect(int trb, time_t timeout);
extern void f_trb_disconnect(int trb);
extern void f_trb_broadcast(unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern void f_trb_acquire(time_t timeout);
#endif
