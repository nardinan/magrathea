/*
 * magrathea
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "magrathea_rms_toolbox.h"
int p_rms_toolbox_mask_channels_apply(int trb, int ladder, int channel, const char *inject_directory) {
	FILE *stream;
	char buffer[d_string_buffer_size], filename[PATH_MAX], trb_code[d_trb_device_hexadecimal_size+1] = {0},
	     command[d_trb_device_hexadecimal_size+1] = {0}, rms_va1[d_trb_device_hexadecimal_size+1] = {0}, rms_va2[d_trb_device_hexadecimal_size+1] = {0};
	unsigned char row_values[d_rms_toolbox_command_row][d_rms_toolbox_command_size], rms_values[d_package_channels_on_dva];
	unsigned short current_crc;
	int index, double_ladder = (ladder/2), double_va = ((ladder%2)*(d_package_channels/d_package_channels_on_dva))+(channel/d_package_channels_on_dva), 
	    va_channel = (channel%d_package_channels_on_va), file_column = ((channel/d_package_channels_on_va)%2), current_row = 0, current_rms = 0,
	    result = d_false;
	snprintf(filename, PATH_MAX, "%s/%s0x%02X%s%02d_%d%s", inject_directory, d_rms_toolbox_filename_prefix, v_package_trbs[trb].code, 
			d_rms_toolbox_filename_postfix, double_ladder, double_va, d_rms_toolbox_filename_extension);
	if ((stream = fopen(filename, "r"))) {
		while (!feof(stream))
			if (fgets(buffer, d_string_buffer_size, stream)) {
				f_string_trim(buffer);
				if (f_string_strlen(buffer) > 0) {
					sscanf(buffer, "%2s %2s %2s %2s", trb_code, command, rms_va1, rms_va2);
					row_values[current_row][0] = (unsigned char)strtol(trb_code, 0, 16);
					row_values[current_row][1] = (unsigned char)strtol(command, 0, 16);
					row_values[current_row][2] = (unsigned char)strtol(rms_va1, 0, 16);
					row_values[current_row][3] = (unsigned char)strtol(rms_va2, 0, 16);
					if ((current_row > 0) && (current_row < (d_rms_toolbox_command_row-1))) {
						rms_values[current_rms] = row_values[current_row][2];
						rms_values[(current_rms+d_package_channels_on_va)] = row_values[current_row][3];
						current_rms++;
					}
					if (++current_row >= d_rms_toolbox_command_row)
						break;
				}
			}
		fclose(stream);
		if (current_row == d_rms_toolbox_command_row) {
			row_values[va_channel+1][(file_column+2)] = 0xFF;
			rms_values[(channel%d_package_channels_on_dva)] = 0xFF;
			current_crc = f_package_crc(rms_values, d_package_channels_on_dva);
			row_values[d_rms_toolbox_command_row-1][2] = (unsigned char)(current_crc>>8);
			row_values[d_rms_toolbox_command_row-1][3] = (unsigned char)(current_crc&0xff);
			if ((stream = fopen(filename, "w"))) {
				for (index = 0; index < d_rms_toolbox_command_row; ++index)
					fprintf(stream, "%02x %02x %02x %02x\n\n", row_values[index][0], row_values[index][1], row_values[index][2],
							row_values[index][3]);
				fclose(stream);
				printf("[masked: TRB %02d | ladder %02d | channel %02d - file %s] row %d, column %d\n", trb, ladder, channel, filename,
						(va_channel+1), (file_column+2));
				result = d_true;
			} else
				d_err(e_log_level_ever, "error, file %s cannot be written", filename);
		} else
			d_err(e_log_level_ever, "error, file %s is damaged", filename);
	} else
		d_err(e_log_level_ever, "error, file not found (unable to mask %d, %d, %d): %s", trb, ladder, channel, filename);
	return result;
}

int f_rms_toolbox_mask_channels(const char *channels_file, const char *inject_directory) {
	FILE *stream;
	char buffer[d_string_buffer_size];
	int trb, ladder, channel, result = d_false;
	if ((stream = fopen(channels_file, "r"))) {
		while (!feof(stream))
			if (fgets(buffer, d_string_buffer_size, stream)) {
				f_string_trim(buffer);
				if (f_string_strlen(buffer) > 0)
					if (sscanf(buffer, "%d %d %d", &trb, &ladder, &channel) == 3)
						p_rms_toolbox_mask_channels_apply(trb, ladder, channel, inject_directory);
			}
		fclose(stream);
		result = d_true;
	}
	return result;
}

int main (int argc, char *argv[]) {
	if (argc > 1) {
		if (f_string_strcmp(argv[1], "-m") == 0) {
			if (argc == 4)
				f_rms_toolbox_mask_channels(argv[3], argv[2]);
			else 
				fprintf(stderr, "masking (-m) usage:\n%s -m <rms_directory> <masking_file>\n", argv[0]);
		}
	} else
		fprintf(stderr, "rms_toolbox usage:\n%s <option> <rms_directory> {arguments}\n"
				"\t (option) -m: mask bad channels", argv[0]);
	return 0;
}
