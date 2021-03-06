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
#include "calibrations.h"
void f_calibrations_values(struct s_calibrations_environment *environment, int trb) {
	int ladder;
	for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
		environment->ladder[trb][ladder].pedestal_mean = f_math_mean(environment->ladder[trb][ladder].pedestal, d_package_channels);
		environment->ladder[trb][ladder].sigma_raw_mean = f_math_mean(environment->ladder[trb][ladder].sigma_raw, d_package_channels);
		environment->ladder[trb][ladder].sigma_mean = f_math_mean(environment->ladder[trb][ladder].sigma, d_package_channels);
		environment->ladder[trb][ladder].pedestal_rms = f_math_rms(environment->ladder[trb][ladder].pedestal, d_package_channels, 0.01);
		environment->ladder[trb][ladder].sigma_raw_rms = f_math_rms(environment->ladder[trb][ladder].sigma_raw, d_package_channels, 0.01);
		environment->ladder[trb][ladder].sigma_rms = f_math_rms(environment->ladder[trb][ladder].sigma, d_package_channels, 0.01);
	}
}

int p_calibrations_file_read_row(char *string, float *pedestal, float *sigma_raw, float *sigma, unsigned short *flags) {
	char *begin_pointer = string, *end_pointer;
	float content[d_calibrations_columns];
	int index = 0;
	while ((index < d_calibrations_columns) && (end_pointer = strchr(begin_pointer, ','))) {
		*end_pointer = '\0';
		content[index++] = atof(begin_pointer);
		begin_pointer = (end_pointer+1);
	}
	if ((index < d_calibrations_columns) && (strlen(begin_pointer) > 0))
		content[index++] = atof(begin_pointer);
	*pedestal = content[3];
	*sigma_raw = content[4];
	*sigma = content[5];
	*flags = (unsigned short)content[6];
	return (int)content[0];
}

int p_calibrations_file_read(struct s_calibrations_environment *environment, FILE *stream, int trb, int ladder) {
	char buffer[d_string_buffer_size], *pointer, *next;
	float pedestal = 0.0f, sigma_raw = 0.0f, sigma = 0.0f;
	unsigned short flag = 0;
	int channel = 0, entries, result = d_true;
	while (!feof(stream))
		if (fgets(buffer, d_string_buffer_size, stream) > 0) {
			for (pointer = buffer, entries = 0; (next = strchr(pointer, ',')); pointer = (next+1), ++entries);
			if (entries >= (d_calibrations_columns-1)) {
				channel = p_calibrations_file_read_row(buffer, &pedestal, &sigma_raw, &sigma, &flag);
				if ((channel >= 0) && (channel < d_package_channels)) {
					environment->ladder[trb][ladder].pedestal[channel] = pedestal;
					environment->ladder[trb][ladder].sigma_raw[channel] = sigma_raw;
					environment->ladder[trb][ladder].sigma[channel] = sigma;
					environment->ladder[trb][ladder].flags[channel] = flag;
				} else
					fprintf(stderr, "warning, channel %d has been readed (but channels have to be between 0 and %d)\n", channel,
							d_package_channels);
			}
		}
	return result;
}

int p_calibrations_file(struct s_calibrations_environment *environment, const char *file) {
	int result = d_false, ladder = 0, trb = 0;
	FILE *stream;
	char *next_pointer, trb_code[3] = {0};
	if ((stream = fopen(file, "r"))) {
		if ((next_pointer = strstr(file, "TRB"))) {
			next_pointer += f_string_strlen("TRB");
			if (*next_pointer == '_')
				next_pointer++;
			trb_code[0] = next_pointer[0];
			trb_code[1] = next_pointer[1];
			trb = atoi(trb_code);
		}
		if ((next_pointer = strrchr(file, '_')))
			ladder = atoi(next_pointer+1);
		if ((ladder >= 0) && (ladder < d_calibrations_ladders))
			result = p_calibrations_file_read(environment, stream, trb, ladder);
		fclose(stream);
	}
	return result;
}

int p_calibrations(struct s_calibrations_environment *environment, const char *directory, int child) {
	DIR *stream;
	struct dirent *descriptor;
	int result = d_false;
	char next_directory[d_string_buffer_size], *next_pointer;
	if ((stream = opendir(directory))) {
		while ((descriptor = readdir(stream)))
			if (descriptor->d_name[0] != '.') {
				snprintf(next_directory, d_string_buffer_size, "%s/%s", directory, descriptor->d_name);
				if (!child)
					p_calibrations(environment, next_directory, d_true);
			}
		closedir(stream);
		result = d_true;
	} else if ((next_pointer = strrchr(directory, '.')) && ((f_string_strcmp(next_pointer, ".cal") == 0) || (f_string_strcmp(next_pointer, ".txt") == 0)))
		result = p_calibrations_file(environment, directory);
	return result;
}

int f_calibrations(struct s_calibrations_environment *environment, const char *directory) {
	return p_calibrations(environment, directory, d_false);
}

void f_calibrations_export(struct s_calibrations_environment *environment, const char *destination, unsigned int trb) {
	char directory[PATH_MAX], filename[PATH_MAX];
	FILE *stream;
	int ladder, channel, va, channel_on_va;
	snprintf(directory, PATH_MAX, "%s_TRB%02d%s", destination, trb, d_calibrations_directory);
	if ((mkdir(directory, 0777) == 0) || (errno == EEXIST))
		for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
			snprintf(filename, PATH_MAX, "%s/TRB_%02d_LAD_%02d.%s", directory, trb, ladder, d_calibrations_extension);
			if ((stream = fopen(filename, "w"))) {
				for (channel = 0; channel < d_package_channels; ++channel) {
					va = channel/d_package_channels_on_va;
					channel_on_va = channel%d_package_channels_on_va;
					fprintf(stream, "%d, %d, %d, %.03f, %.03f, %.03f, %d\n", channel, va, channel_on_va,
							environment->ladder[trb][ladder].pedestal[channel], environment->ladder[trb][ladder].sigma_raw[channel],
							environment->ladder[trb][ladder].sigma[channel], environment->ladder[trb][ladder].flags[channel]);
				}
				fclose(stream);
			}
		}
}

