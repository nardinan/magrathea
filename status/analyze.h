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
#ifndef magrathea_analyze_h
#define magrathea_analyze_h
#include "../trb_device.h"
#define d_analyze_csv_elements (e_trb_device_currents_null+e_trb_device_temperatures_null+d_trb_device_temperatures_size\
		+e_trb_device_voltages_null+e_trb_device_status_null+2)
#define d_analyze_row_size 2048
extern struct s_trb_status v_current_status;
extern int p_analyze_next_split(char *buffer, char divisor, size_t elements_size, char tokens[elements_size][d_string_buffer_size]);
extern int p_analyze_next_code(size_t elements_size, char tokens[elements_size][d_string_buffer_size], unsigned char trb_code, time_t *timestamp);
extern int f_analyze_next(FILE *stream, unsigned char trb_code, time_t *timestamp, time_t starting_timestamp);
#endif
