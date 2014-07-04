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
#include "adlink.h"
struct s_adlink v_adlink_system;
int f_adlink_connect(enum e_adlink_boards board) {
	unsigned int value;
	int result = d_false;
	switch (board) {
		case e_adlink_boards_trigger:
			if (!v_adlink_system.trigger_connected)
				if ((v_adlink_system.trigger_connected = (f_plx_select(d_adlink_vendor_code, d_adlink_product_code_trigger,
							&(v_adlink_system.trigger_device)) == ApiSuccess)))
					PlxPci_DeviceReset(&(v_adlink_system.trigger_device));
			result = v_adlink_system.trigger_connected;
			break;
		case e_adlink_boards_data:
			if (!v_adlink_system.data_connected)
				if (f_plx_select(d_adlink_vendor_code, d_adlink_product_code_data, &(v_adlink_system.data_device)) == ApiSuccess) {
					if (f_plx_prepare_dma(&(v_adlink_system.data_device), 0, &(v_adlink_system.device_buffer),
								&(v_adlink_system.notification), &(v_adlink_system.raw_memory_link)) == ApiSuccess) {
						f_plx_write_register(&(v_adlink_system.data_device), (d_trb_device_boards*8), (0x3|0x55AA<<16|2000<<4));
						f_plx_write_register(&(v_adlink_system.data_device), (d_trb_device_boards*8), (0x2|0x55AA<<16|2000<<4));
						f_plx_write_register(&(v_adlink_system.data_device), (d_trb_device_boards*8), (0x0|0x55AA<<16|2000<<4));
						f_plx_read_register(&(v_adlink_system.data_device), (d_trb_device_boards*8), &value);
						v_adlink_system.data_connected = d_true;
					} else
						f_plx_destroy(&(v_adlink_system.data_device), 0, &(v_adlink_system.notification),
								&(v_adlink_system.raw_memory_link));
				}
			result = v_adlink_system.data_connected;
	}
	return result;
}

void f_adlink_destroy(enum e_adlink_boards board) {
	switch (board) {
		case e_adlink_boards_trigger:
			if (v_adlink_system.trigger_connected) {
				f_plx_destroy(&(v_adlink_system.trigger_device), -1, NULL, NULL);
				v_adlink_system.trigger_connected = d_false;
			}
			break;
		case e_adlink_boards_data:
			if (v_adlink_system.data_connected) {
				f_plx_destroy(&(v_adlink_system.data_device), 0, &(v_adlink_system.notification), &(v_adlink_system.raw_memory_link));
				v_adlink_system.data_connected = d_false;
			}
	}
}

int f_adlink_trigger_setup(enum e_adlink_trigger trigger) {
	struct {
		unsigned int default_level, type, repeat_counter, width, interval;
	} pulse_properties = {
		/* Zhangfei's configuration */
		.default_level = 1,
		.type = 2,
		.repeat_counter = (1<<12),
		.width = 5 /* 5 * 0.1uS */
	};
	unsigned int value;
	int speed = e_adlink_trigger_50, result = d_false;
	if (f_adlink_connect(e_adlink_boards_trigger)) {
		if (trigger != e_adlink_trigger_disabled) {
			if (trigger != e_adlink_trigger_external)
				speed = trigger;
			pulse_properties.interval = (1E7/speed)-pulse_properties.width;
			value = ((pulse_properties.default_level&0x0001)<<31)|
				((pulse_properties.type&0x0003)<<29)|
				((pulse_properties.repeat_counter&0x1FFF)<<16)|
				((pulse_properties.width&0x7FFF));
			if (trigger == e_adlink_trigger_external)
				value |= 0x8000;
			f_plx_write_register(&(v_adlink_system.trigger_device), 40, pulse_properties.interval);
			f_plx_write_register(&(v_adlink_system.trigger_device), 44, value);
			f_plx_write_register(&(v_adlink_system.trigger_device), 0, 0x00);
		} else
			f_plx_write_register(&(v_adlink_system.trigger_device), 0, 0xFF);
		result = d_true;
	}
	return result;
}

int f_adlink_data_read(unsigned int trb, size_t *readed, FILE *destination, time_t timeout) {
	int result = d_false;
	if (f_adlink_connect(e_adlink_boards_data)) {
		if (f_plx_read(&(v_adlink_system.data_device), d_adlink_bucket_size, readed, &(v_adlink_system.device_buffer), &(v_adlink_system.notification),
					(d_trb_device_boards*trb), (d_trb_device_boards*trb)+4, 0, timeout) == ApiSuccess) {
			if (((*readed) > 0) && (destination)) {
				fwrite(v_adlink_system.raw_memory_link, (*readed), 1, destination);
				fflush(destination);
			}
			result = d_true;
		} else
			f_adlink_destroy(e_adlink_boards_data);
	}
	return result;
}

