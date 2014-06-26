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
#define d_trb_command_size 4
#define d_trb_raw_command_size 12
extern int v_trb_descriptor;
extern unsigned char v_trb_raw_head[], v_trb_raw_tail[];
extern size_t v_trb_sentinel_size;
extern void f_trb_command_packet(unsigned char *supplied, unsigned char trb_code, unsigned char type, unsigned char high_byte, unsigned char low_byte);
#endif
