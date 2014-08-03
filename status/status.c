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
#include "status.h"
struct s_trb_status current_status;
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
	if ((strlen(pointer) > 0) && (elements < elements_size))
		strncpy(tokens[elements++], pointer, d_string_buffer_size);
	return elements;
}

void f_analyze_next_code(size_t elements_size, char tokens[elements_size][d_string_buffer_size], unsigned char trb_code) {
	unsigned char current_code;
	int local_index = 0, index;
	if (elements_size >= d_status_csv_elements) {
		local_index++;
		if ((current_code = (unsigned char)strtol(tokens[local_index], NULL, 16)) == trb_code) {
			local_index++;
			for (index = 0; index < e_trb_device_currents_null; ++index, ++local_index)
				current_status.currents[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_temperatures_null; ++index, ++local_index)
				current_status.temperatures[index] = atof(tokens[local_index]);
			for (index = 0; index < d_trb_device_temperatures_size; ++index, ++local_index)
				current_status.tfh_temperatures[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_voltages_null; ++index, ++local_index)
				current_status.voltages[index] = atof(tokens[local_index]);
			for (index = 0; index < e_trb_device_status_null; ++index, ++local_index)
				current_status.status[index] = (unsigned char)strtol(tokens[local_index], NULL, 16);
		}
	}
}

int f_analyze_next(FILE *stream, unsigned char trb_code) {
	int elements, result = d_false;
	char buffer[d_status_row_size], tokens[d_status_csv_elements][d_string_buffer_size];
	if (fgets(buffer, d_status_row_size, stream) > 0) {
		if ((elements = p_analyze_next_split(buffer, d_trb_device_csv_character, d_status_csv_elements, tokens)) > 0) {
			/* remove me - DEBUG */
			printf("[init]\n");
			for (; elements >= 0; --elements)
				printf("%s\n", tokens[elements]);
			printf("[end]\n");
			/* end - DEBUG */
			f_analyze_next_code(elements, tokens, trb_code);
		}
	}
	return result;
}

int main (int argc, char *argv[]) {
	FILE *stream = NULL;
	if (argc == 2) {
		if ((stream = fopen(argv[1], "r"))) {
			while (d_true)
				f_analyze_next(stream, 0x01);
		} else
			fprintf(stderr, "404 - file %s not found\n", argv[1]);
	} else
		fprintf(stderr, "usage:\n%s <file>\n", argv[0]);
	return 0;
}


