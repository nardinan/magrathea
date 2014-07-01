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
#define d_trb_read_timeout 250
#define d_trb_command_size 4
#define d_trb_raw_command_size 12
#define d_trb_sentinel_size 2
#define d_trb_boards 8
typedef struct s_trb {
	/* + initialized values */
	int descriptor, ready:1, selected:1;
	unsigned char code;
	const char *location;
	/* - initialized values */
	struct termios old_configuration;
	char destination[d_string_buffer_size];
	FILE *stream;
	size_t written_bytes;
} s_trb;
extern struct s_trb v_trb_boards[d_trb_boards];
extern unsigned char v_trb_raw_head[d_trb_sentinel_size], v_trb_raw_tail[d_trb_sentinel_size];
extern void f_trb_disconnect(int trb);
extern void f_trb_wake_up(time_t timeout);
extern void f_trb_apply_mask(unsigned char flag);
extern void f_trb_command_packet(unsigned char *supplied, unsigned char trb, unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern void f_trb_broadcast(unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern int f_trb_set_stream(int trb, const char *destination);
extern void f_trb_acquire(time_t timeout);
#endif
