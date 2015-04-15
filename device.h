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
#ifndef magrathea_device_h
#define magrathea_device_h
#include <miranda/ground.h>
typedef enum e_device_calls {
	e_device_calls_trigger = 0,
	e_device_calls_description,
	e_device_calls_status,
	e_device_calls_stream,
	e_device_calls_write,
	e_device_calls_read,
	e_device_calls_mask,
	e_device_calls_focus,
	e_device_calls_view,
	e_device_calls_convert,
	e_device_calls_inject,
	e_device_calls_null
} e_device_calls;
typedef enum e_device_system_calls {
	e_device_system_calls_is_enabled = 0,
	e_device_system_calls_initialize,
	e_device_system_calls_destroy,
	e_device_system_calls_null
} e_device_system_calls;
typedef int (*t_device_call_generic)(unsigned char);
typedef int (*t_device_call)(unsigned char, char **, size_t, int output);
typedef int (*t_device_call_refresh)(unsigned char, struct s_console *);
typedef struct s_device {
	unsigned char code;
	char *description;
	t_device_call calls[e_device_calls_null];
	t_device_call_generic system_calls[e_device_system_calls_null];
	t_device_call_refresh refresh_call;
} s_device;
extern struct s_device *v_devices;
extern const char *v_device_call_description[], *v_device_system_call_description[];
extern int f_device_recall(enum e_device_calls call, int skip_mask, char **tokens, size_t elements, int output);
extern int f_device_system_recall(enum e_device_system_calls call, int output);
extern int f_device_system_refresh(struct s_console *console);
#endif
