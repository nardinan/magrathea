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
#include "lvds_device.h"
int f_lvds_device_refresh(unsigned char code) {
	int index, result = d_true;
	size_t readed;
	for (index = 0; index < d_trb_device_boards; index++)
		if ((v_trb_device_boards[index].descriptor != d_rs232_null) &&
				(f_adlink_data_read(index, &readed, v_trb_device_boards[index].stream.stream, d_lvds_device_timeout)))
			if (v_trb_device_boards[index].stream.stream)
				v_trb_device_boards[index].stream.written_bytes += readed;
	return result;
}

int f_lvds_device_initialize(unsigned char code) {
	return f_adlink_connect(e_adlink_boards_data);
}

int f_lvds_device_destroy(unsigned char code) {
	int result = d_true;
	f_adlink_destroy(e_adlink_boards_data);
	return result;
}


