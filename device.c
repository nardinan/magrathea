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
#include "device.h"
struct s_device *v_devices = NULL;
const char *v_device_call_description[] = {
	"trigger",
	"description",
	"status",
	"stream",
	"write",
	"read",
	"mask",
	"null"
}, *v_device_system_call_description[] = {
	"enabled",
	"initialize",
	"destroy"
};
int f_device_recall(e_device_calls call, int skip_mask, char **tokens, size_t elements, int output) {
	int index, result = 0;
	char buffer[d_string_buffer_size];
	if (v_devices)
		for (index = 0; v_devices[index].code != 0xff; index++)
			if (v_devices[index].calls[call])
				if ((!v_devices[index].system_calls[e_device_system_calls_is_enabled]) || (skip_mask) ||
						(v_devices[index].system_calls[e_device_system_calls_is_enabled](v_devices[index].code))) {
					if (v_devices[index].calls[call](v_devices[index].code, tokens, elements, output))
						result++;
					else if (output != d_console_descriptor_null) {
						snprintf(buffer, d_string_buffer_size, "%sfailed%s running command %s%s%s on device %s\n",
								v_console_styles[e_console_style_red], v_console_styles[e_console_style_reset],
								v_console_styles[e_console_style_bold], v_device_call_description[call],
								v_console_styles[e_console_style_reset], v_devices[index].description);
						write(output, buffer, f_string_strlen(buffer));
						fsync(output);
					}
				}
	return result;
}

int f_device_system_recall(e_device_system_calls call, int output) {
	int index, result = d_true;
	char buffer[d_string_buffer_size];
	if (v_devices)
		for (index = 0; v_devices[index].code != 0xff; index++)
			if (v_devices[index].system_calls[call])
				if (!v_devices[index].system_calls[call](v_devices[index].code)) {
					result = d_false;
					if (output != d_console_descriptor_null) {
						snprintf(buffer, d_string_buffer_size, "%sfailed%s running command %s%s%s on device %s\n",
								v_console_styles[e_console_style_red], v_console_styles[e_console_style_reset],
								v_console_styles[e_console_style_bold], v_device_system_call_description[call],
								v_console_styles[e_console_style_reset], v_devices[index].description);
						write(output, buffer, f_string_strlen(buffer));
						fsync(output);
					}
				}
	return result;
}

int f_device_system_refresh(struct s_console *console) {
	int index, result = d_true;
	if (v_devices)
		for (index = 0; v_devices[index].code != 0xff; index++)
			if (v_devices[index].refresh_call)
				if (!v_devices[index].refresh_call(v_devices[index].code, console))
					result = d_false;
	return result;
}


