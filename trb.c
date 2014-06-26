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
int v_trb_descriptor = d_rs232_null;
unsigned char v_trb_raw_head[] = {0x55, 0xaa}, v_trb_raw_tail[] = {0x5a, 0xa5};
size_t v_trb_sentinel_size = 2;
void f_trb_command_packet(unsigned char *supplied, unsigned char trb_code, unsigned char type, unsigned char high_byte, unsigned char low_byte) {
	static unsigned char raw_command[d_trb_raw_command_size] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned int checksum = (trb_code+type+high_byte+low_byte);
	memcpy(supplied, raw_command, d_trb_raw_command_size);
	supplied[4] = trb_code;
	supplied[5] = type;
	supplied[6] = high_byte;
	supplied[7] = low_byte;
	supplied[8] = (checksum>>8)&0xff;
	supplied[9] = (checksum)&0xff;
}

