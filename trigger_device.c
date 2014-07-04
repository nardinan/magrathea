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
#include "trigger_device.h"
int f_trigger_device_trigger(unsigned char code, char **tokens, size_t elements, int output) {
	char buffer[d_string_buffer_size];
	enum e_adlink_trigger trigger = e_adlink_trigger_50;
	int argument, result = d_false, speed = 0;
	if (f_console_parameter("-off", tokens, elements, d_true) != d_console_parameter_null)
		trigger = e_adlink_trigger_disabled;
	else if (f_console_parameter("-ext", tokens, elements, d_true) != d_console_parameter_null)
		trigger = e_adlink_trigger_external;
	else if ((argument = f_console_parameter("-s", tokens, elements, d_false)) != d_console_parameter_null)
		switch ((speed = atoi(tokens[argument]))) {
			case 100:
				trigger = e_adlink_trigger_100;
				break;
			case 200:
				trigger = e_adlink_trigger_200;
				break;
			case 300:
				trigger = e_adlink_trigger_300;
				break;
			case 50:
			default:
				trigger = e_adlink_trigger_50;
		}
	result = f_adlink_trigger_setup(trigger);
	if (output != d_console_descriptor_null) {
		if (result)
			switch (trigger) {
				case e_adlink_trigger_disabled:
					strncpy(buffer, "trigger has been disabled\n", d_string_buffer_size);
					break;
				case e_adlink_trigger_external:
					strncpy(buffer, "running external trigger\n", d_string_buffer_size);
					break;
				case e_adlink_trigger_100:
				case e_adlink_trigger_200:
				case e_adlink_trigger_300:
				case e_adlink_trigger_50:
				default:
					snprintf(buffer, d_string_buffer_size, "running trigger @ %dHz\n", trigger);
			}
		else
			snprintf(buffer, d_string_buffer_size, "%sYEK!%s 404 of trigger board\n", v_console_styles[e_console_style_red],
					v_console_styles[e_console_style_reset]);
		write(output, buffer, f_string_strlen(buffer));
		fsync(output);
	}
	return result;
}

int f_trigger_device_initialize(unsigned char code) {
	return f_adlink_connect(e_adlink_boards_trigger);
}

int f_trigger_device_destroy(unsigned char code) {
	int result = d_true;
	f_adlink_destroy(e_adlink_boards_trigger);
	return result;
}
