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
#include "package.h"
unsigned char *p_package_analyze_raw(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *pointer = buffer, *result = NULL;
	int index, ladder;
	if (size >= d_package_raw_size) {
		for (ladder = 0; ladder < d_package_ladders; ++ladder) {
			package->data.values.raw.ladder[ladder] = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			pointer += 2;
			for (index = 0; index < d_package_channels; ++index, pointer += 2)
				package->data.values.raw.values[ladder][index] = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
		}
		result = pointer;
	}
	return result;
}

unsigned char *p_package_analyze_header_data(struct s_package *package, unsigned char *buffer, size_t size) {
	static unsigned char header_data[] = {0xEE, 0xBB};
	unsigned char *pointer = buffer, *backup, *result = NULL;
	int index;
	/*while ((size >= d_package_data_header_size) && ((*pointer) != header_data[0])) {
		pointer++;
		size--;
	}*/
	if (size >= d_package_data_header_size) {
		for (index = 0; index < d_package_data_header_const_size; ++index, ++pointer)
			if (*pointer != header_data[index])
				break;
		if (index >= d_package_data_header_const_size) {
			package->data.kind = pointer[0];
			pointer++;
			package->data.trb = pointer[0];
			pointer++;
			package->data.frame_length = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			pointer += 2;
			backup = p_package_analyze_raw(package, pointer, (size-(pointer-buffer)));
			if ((backup) && ((size-(backup-buffer)) >= d_package_data_tail_size)) {
				pointer = backup;
				package->data.trigger = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
				pointer += 2;
				package->data.sumcheck = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
				result = (pointer+2);
			}
		} else
			package->damaged = d_true;
	}
	return result;
}

unsigned char *p_package_analyze_header(struct s_package *package, unsigned char *buffer, size_t size) {
	static unsigned char header_frame[] = {0xEB, 0x90};
	unsigned char *pointer = buffer, *backup, *result = NULL;
	int index;
	if (size >= d_package_frame_header_size) {
		for (index = 0; index < d_package_frame_header_const_size; ++index, ++pointer)
			if (*pointer != header_frame[index])
				break;
		if (index >= d_package_frame_header_const_size) {
			package->count = pointer[0];
			pointer++;
			package->trb = pointer[0];
			pointer++;
			package->frame_length = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			pointer += 2;
			backup = p_package_analyze_header_data(package, pointer, (size-(pointer-buffer)));
			if ((backup) && ((size-(backup-buffer)) >= d_package_frame_tail_size)) {
				pointer = backup;
				package->sumcheck = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
				result = (pointer+2);
			}
		} else
			package->damaged = d_true;
	}
	return result;
}

unsigned char *p_package_analyze(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *result = NULL;
	package->complete = d_false;
	if ((result = p_package_analyze_header(package, buffer, size)))
		package->complete = d_true;
	return result;
}

unsigned char *f_package_analyze(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *pointer = buffer, *backup;
	while (size > 0) {
		package->damaged = d_false;
		if (!(backup = p_package_analyze(package, pointer, size))) {
			if (package->damaged) {
				printf("damaged\n");
				pointer++;
				size--;
			} else
				break;
		} else {
			pointer = backup;
			break;
		}
	}
	return pointer;
}

