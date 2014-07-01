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
unsigned char v_trb_raw_head[d_trb_sentinel_size] = {0x55, 0xaa}, v_trb_raw_tail[d_trb_sentinel_size] = {0x5a, 0xa5};
void f_trb_disconnect(int trb) {
	if (v_trb_boards[trb].ready) {
		f_rs232_close_termios(v_trb_boards[trb].descriptor, v_trb_boards[trb].old_configuration);
		v_trb_boards[trb].descriptor = d_rs232_null;
		v_trb_boards[trb].ready = d_false;
	}
}

void f_trb_wake_up(time_t timeout) {
	static unsigned char bus_loopback[d_trb_raw_command_size] = {0x55, 0xaa, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5};
	unsigned char loopback_reply[d_trb_raw_command_size];
	int index;
	for (index = 0; index < d_trb_boards; ++index) {
		if (!v_trb_boards[index].ready)
			v_trb_boards[index].ready = f_rs232_open(v_trb_boards[index].location, e_rs232_baud_115200, e_rs232_bits_8, e_rs232_stops_2_bit,
					e_rs232_parity_odd, e_rs232_flow_control_no, &(v_trb_boards[index].descriptor),
					&(v_trb_boards[index].old_configuration));
		if (v_trb_boards[index].ready) {
			f_rs232_write(v_trb_boards[index].descriptor, bus_loopback, d_trb_raw_command_size);
			if (f_rs232_read_packet(v_trb_boards[index].descriptor, loopback_reply, d_trb_raw_command_size, timeout,
						v_trb_raw_head, v_trb_raw_tail, d_trb_sentinel_size) <= 0)
				f_trb_disconnect(index);
		}
	}
}

void f_trb_apply_mask(unsigned char flags) {
	int index;
	for (index = 0; index < d_trb_boards; ++index, flags >> 1)
		if (v_trb_boards[index].ready)
			v_trb_boards[index].selected = ((flags&0x1));
}

void f_trb_command_packet(unsigned char *supplied, unsigned char trb, unsigned char type, unsigned char high_byte, unsigned char low_byte) {
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

void f_trb_broadcast(unsigned char type, unsigned char high_byte, unsigned char low_byte) {
	unsigned char raw_command[d_trb_raw_command_size];
	int index;
	for (index = 0; index < d_trb_boards; ++index)
		if ((v_trb_boards[index].ready) && (v_trb_boards[index].selected)) {
			f_trb_command_packet(raw_command, v_trb_boards[index].code, type, high_byte, low_byte);
			if (f_rs232_write(v_trb_boards[index].descriptor, raw_command, d_trb_raw_command_size) != d_trb_raw_command_size)
				f_trb_disconnect(index);
		}
}

int f_trb_trigger(enum e_trb_trigger trigger) {
	struct {
		unsigned int default_level, type, repeat_counter, width, interval;
	} pulse_properties = {
		/* real Zhangfei's configuration */
		.default_level = 1,
		.type = 2,
		.repeat_counter = (1<<12),
		.width = 5
	};
	PLX_DEVICE_OBJECT trigger_device;
	unsigned int value;
	int speed = e_trb_trigger_50, result = d_false;
	if ((trigger != e_trb_trigger_disabled) && (trigger != e_trb_trigger_external))
		speed = trigger;
	if (f_plx_select(d_trb_vendor_code, d_trb_product_code_trigger, &trigger_device) == ApiSuccess) {
		pulse_properties.interval = (1E7/speed)-pulse_properties.width;
		if (trigger != e_trb_trigger_disabled) {
			f_plx_write_register(&trigger_device, 40, pulse_properties.interval);
			value = ((pulse_properties.default_level&0x0001)<<31)|
				((pulse_properties.type&0x0003)<<29)|
				((pulse_properties.repeat_counter&0x1FFF)<<16)|
				((pulse_properties.width&0x7FFF));
			if (trigger == e_trb_trigger_external)
				value |= 0x8000;
			f_plx_write_register(&trigger_device, 44, value);
			f_plx_write_register(&trigger_device, 0, 0x00);
		} else
			f_plx_write_register(&trigger_device, 0, 0x0F);
		f_plx_destroy(&trigger_device, -1, NULL, NULL);
		result = d_true;
	}
	return result;
}

