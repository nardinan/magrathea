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
#include "analyze.h"
struct s_trb_status v_current_status;
int p_analyze_next_split(char *buffer, char divisor, size_t elements_size, char tokens[elements_size][d_string_buffer_size]) {
	int elements = 0;
	char *pointer = buffer, *next;
	while ((next = strchr(pointer, divisor))) {
		if ((next > pointer) && (elements < elements_size)) {
			*next = '\0';
			strncpy(tokens[elements++], pointer, d_string_buffer_size);
			pointer = next+1;
		} else
			break;
	}
	if (((strlen(pointer) > 0) && (elements < elements_size)))
		strncpy(tokens[elements++], pointer, d_string_buffer_size);
	return elements;
}

int p_analyze_next_code(size_t elements_size, char tokens[elements_size][d_string_buffer_size], unsigned char trb_code, time_t *timestamp) {
	unsigned char current_code;
	int local_index = 0, index, result = d_false;
	time_t current_timestamp;
	if (elements_size >= d_analyze_csv_elements) {
		current_timestamp = atol(tokens[local_index]);
		local_index++;
		if ((current_code = (unsigned char)strtol(tokens[local_index], NULL, 16)) == trb_code) {
			local_index++;
			*timestamp = current_timestamp;
			for (index = 0; index < e_trb_device_currents_null; ++index, ++local_index)
				v_current_status.currents[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_temperatures_null; ++index, ++local_index)
				v_current_status.temperatures[index] = atof(tokens[local_index]);
			for (index = 0; index < d_trb_device_temperatures_size; ++index, ++local_index)
				v_current_status.tfh_temperatures[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_voltages_null; ++index, ++local_index)
				v_current_status.voltages[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_status_null; ++index, ++local_index)
				v_current_status.status[index] = (unsigned char)strtol(tokens[local_index], NULL, 16);
			result = d_true;
		}
	}
	return result;
}

int f_analyze_next(FILE *stream, unsigned char trb_code, time_t *timestamp) {
	int elements, result = d_false;
	char buffer[d_analyze_row_size], tokens[d_analyze_csv_elements][d_string_buffer_size];
	if (fgets(buffer, d_analyze_row_size, stream) > 0)
		if ((elements = p_analyze_next_split(buffer, d_trb_device_csv_character, d_analyze_csv_elements, tokens)) == d_analyze_csv_elements)
			result = p_analyze_next_code(elements, tokens, trb_code, timestamp);
	return result;
}

