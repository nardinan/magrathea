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
struct s_package_trb v_package_trbs[] = {
	{0, 0x00},
	{1, 0x01},
	{2, 0x08},
	{3, 0x09},
	{4, 0x04},
	{5, 0x05},
	{6, 0x0C},
	{7, 0x0D}
};
char *v_package_kind[] = {
	"damaged workmode", 	/* 0 */
	"unused workmode",	/* 1 */
	"compressed workmode",	/* 2 */
	"raw workmode",		/* 3 */
	"unused workmode",	/* 4 */
	"unused workmode",	/* 5 */
	"download workmode",	/* 6 */
	"unused wormkode",	/* 7 */
	"timestamp workmode"	/* 8 */
};
unsigned short p_package_crc_result(unsigned char *buffer) {
	int index;
	unsigned short result = 0;
	for (index = 0; index < d_package_crc_register; ++index)
		result |= (buffer[index]<<index);
	return result;
}

unsigned short f_package_crc(unsigned char *buffer, size_t size) {
	int index, local_bit;
	unsigned char crc_register[d_package_crc_register], exponent;
	memset(crc_register, 1, d_package_crc_register);
	for (index = 0; index < size; ++index)
		for (local_bit = 0; local_bit < d_package_bits_byte; ++local_bit) {
			exponent = crc_register[15]^((buffer[index]>>(7-local_bit))&0x1);
			crc_register[15] = crc_register[14];
			crc_register[14] = crc_register[13];
			crc_register[13] = crc_register[12];
			crc_register[12] = crc_register[11]^exponent;
			crc_register[11] = crc_register[10];
			crc_register[10] = crc_register[9];
			crc_register[9] = crc_register[8];
			crc_register[8] = crc_register[7];
			crc_register[7] = crc_register[6];
			crc_register[6] = crc_register[5];
			crc_register[5] = crc_register[4]^exponent;
			crc_register[4] = crc_register[3];
			crc_register[3] = crc_register[2];
			crc_register[2] = crc_register[1];
			crc_register[1] = crc_register[0];
			crc_register[0] = exponent;
		}
	return p_package_crc_result(crc_register);
}

unsigned char *p_package_analyze_nrm(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *pointer = buffer;
	int ladder = -1, entry_point = -1;
	memset(&(package->data.values.nrm), 0, sizeof(union u_package_data_values));
	if (*pointer == d_package_nrm_ladder) {
		while (size) {
			if (pointer[0] == d_package_nrm_ladder)
				ladder = pointer[1];
			else if ((pointer[0]&d_package_nrm_cluster) == d_package_nrm_cluster)
				entry_point = ((unsigned short int)pointer[1])|((unsigned short int)(pointer[0]&0x03))<<8;
			else if (((ladder >= 0) && (ladder < d_trb_device_ladders)) && ((entry_point >= 0) && (entry_point < d_package_channels)))
				package->data.values.nrm.ladders_data[ladder].values[entry_point++] =
					((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			else
				break;
			pointer += 2;
			size -= 2;
		}
	}
	return pointer;
}

unsigned char *p_package_analyze_dld(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *pointer = buffer, *result = NULL;
	int index, ladder;
	if (size >= d_package_raw_size) {
		for (ladder = 0; ladder < d_package_ladders; ++ladder) {
			package->data.values.dld.ladder[ladder] = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			pointer += 2;
			for (index = 0; index < d_package_channels; ++index, pointer += 2) {
				package->data.values.dld.pedestal[ladder][index] = pointer[0];
				package->data.values.dld.rms[ladder][index] = pointer[1];
			}
		}
		result = pointer;
	}
	return result;
}

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
	unsigned char *pointer = buffer, *backup = NULL, *result = NULL, workmode;
	int index;
	if (size >= d_package_alignment_size)
		for (index = 0; index < d_package_alignment_size; ++index) {
			if (*pointer != header_data[0]) {
				pointer++;
				size--;
			} else
				break;
		}
	if (size >= d_package_data_header_size) {
		for (index = 0; index < d_package_data_header_const_size; ++index, ++pointer)
			if (*pointer != header_data[index])
				break;
		if (index >= d_package_data_header_const_size) {
			if ((size-(pointer-buffer)) > d_package_data_header_info_size) {
				workmode = (pointer[0]>>4)&0x0f;
				//group = (pointer[0])&0x0f; // not needed
				if ((workmode == d_package_raw_workmode) || (workmode == d_package_nrm_workmode) || (workmode == d_package_dld_workmode)) {
					package->data.kind = workmode;
					pointer++;
					package->data.trb = (pointer[0]&0x3f);
					pointer++;
					package->data.frame_length = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
					pointer += 2;
					switch (workmode) {
						case d_package_raw_workmode:
							backup = p_package_analyze_raw(package, pointer, (size-(pointer-buffer)));
							break;
						case d_package_dld_workmode:
							backup = p_package_analyze_dld(package, pointer, (size-(pointer-buffer)));
							break;
						case d_package_nrm_workmode:
							if ((size-(pointer-buffer)) >= package->data.frame_length)
								backup = p_package_analyze_nrm(package, pointer,
										(package->data.frame_length-d_package_data_tail_size-
										 d_package_frame_tail_size));
					}
					if ((backup) && ((size-(backup-buffer)) >= d_package_data_tail_size)) {
						pointer = backup;
						pointer++;
						pointer++;
						package->data.trigger_kind = ((unsigned short int)pointer[0])>>5;
						package->data.trigger_counter = ((unsigned short int)pointer[1])|((unsigned short int)(pointer[0]&0xf))<<8;
						pointer += 2;
						package->data.sumcheck = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
						result = (pointer+2);
					}
				} else
					package->damaged = d_true;
			}
		} else
			package->damaged = d_true;
	}
	return result;
}

unsigned char *p_package_analyze_header(struct s_package *package, unsigned char *buffer, size_t size) {
	static unsigned char header_frame[] = {0xEB, 0x90};
	unsigned char *pointer = buffer, *backup, *result = NULL, *crc_started;
	int index;
	if (size >= d_package_frame_header_size) {
		for (index = 0; index < d_package_frame_header_const_size; ++index, ++pointer)
			if (*pointer != header_frame[index])
				break;
		if (index >= d_package_frame_header_const_size) {
			if ((size-(pointer-buffer)) > d_package_frame_header_info_size) {
				crc_started = pointer;
				package->count = pointer[0];
				pointer++;
				package->trb = (pointer[0]&0x3f);
				pointer++;
				package->frame_length = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
				pointer += 2;
				backup = p_package_analyze_header_data(package, pointer, (size-(pointer-buffer)));
				if ((backup) && ((size-(backup-buffer)) >= d_package_frame_tail_size)) {
					pointer = backup;
					package->sumcheck = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
					package->real_sumcheck = f_package_crc(crc_started, pointer-crc_started);
					//package->wrong_sumcheck = (package->real_sumcheck != package->sumcheck);
					result = (pointer+2);
				}
			}
		} else
			package->damaged = d_true;
	}
	return result;
}

unsigned char *p_package_analyze_timestamp(struct s_package *package, unsigned char *buffer, size_t size) {
	static unsigned char header_frame[] = {0xE2, 0x25, 0x08, 0x13};
	unsigned char *pointer = buffer, *result = NULL;
	int index;
	if (size >= d_package_timestamp_size) {
		for (index = 0; index < d_package_timestamp_header_const_size; ++index, ++pointer)
			if (*pointer != header_frame[index])
				break;
		if (index >= d_package_timestamp_header_const_size) {
			package->data.kind = d_package_tmp_workmode;
			package->data.trigger_counter = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			pointer += d_package_timestamp_tail_const_size;
			package->data.values.tmp.seconds = ((unsigned long long)pointer[5])|(((unsigned long long)pointer[4])<<8)|
				(((unsigned long long)pointer[3])<<16)|(((unsigned long long)pointer[2])<<24)|
				(((unsigned long long)pointer[1])<<32)|(((unsigned long long)pointer[0])<<40);
			pointer += 6;
			package->data.values.tmp.mseconds = ((unsigned short int)pointer[1])|((unsigned short int)pointer[0])<<8;
			result = (pointer+2);
		} else
			package->damaged = d_true;
	} else
		package->damaged = d_true;
	return result;
}

unsigned char *p_package_analyze(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *result = NULL;
	if (((result = p_package_analyze_header(package, buffer, size))) || ((result = p_package_analyze_timestamp(package, buffer, size)))) {
		package->damaged = d_false;
		package->complete = d_true;
	} else
		package->complete = d_false;
	return result;
}

unsigned char *f_package_analyze(struct s_package *package, unsigned char *buffer, size_t size) {
	unsigned char *pointer = buffer, *backup;
	while (size > 0) {
		package->damaged = d_false;
		package->wrong_sumcheck = d_false;
		if (!(backup = p_package_analyze(package, pointer, size))) {
			if (package->damaged) {
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

