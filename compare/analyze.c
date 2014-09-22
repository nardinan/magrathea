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
void f_analyze_values_write(struct s_analyze_environment *environment, FILE *stream) {
	int calibration, channel, ladder, bad_channels;
	for (ladder = 0; ladder < d_analyze_ladders; ++ladder) {
		bad_channels = 0;
		for (channel = 0; channel < d_package_channels; ++channel)
			if (fabs(environment->pedestal_distance[ladder][channel]) >= d_analyze_pedestal_distance_max)
				bad_channels++;
		fprintf(stream, "ladder %02d [different channels %s%d\x1B[0m]", ladder, (bad_channels>0)?"\x1B[31m":"\x1B[32m", bad_channels);
		fprintf(stream, "\tPED ");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), environment->calibration[calibration].ladder[ladder].pedestal_mean,
				environment->calibration[calibration].ladder[ladder].pedestal_rms);
		fputc(']', stream);
		fprintf(stream, "\tSRAW");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), environment->calibration[calibration].ladder[ladder].sigma_raw_mean,
				environment->calibration[calibration].ladder[ladder].sigma_raw_rms);
		fputc(']', stream);
		fprintf(stream, "\tSIG ");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), environment->calibration[calibration].ladder[ladder].sigma_mean,
				environment->calibration[calibration].ladder[ladder].sigma_rms);
		fprintf(stream, "]\n");
		fflush(stream);
	}
}

void f_analyze_values(struct s_analyze_environment *environment) {
	int calibration, channel, ladder, value;
	for (ladder = 0; ladder < d_analyze_ladders; ++ladder) {
		for (channel = 0; channel < d_package_channels; ++channel) {
			value = environment->calibration[0].ladder[ladder].pedestal[channel];
			for (calibration = 1; calibration < d_analyze_calibrations; ++calibration)
				value -= environment->calibration[calibration].ladder[ladder].pedestal[channel];
			environment->pedestal_distance[ladder][channel] = value;
		}
	}
	for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
		for (ladder = 0; ladder < d_analyze_ladders; ++ladder) {
			environment->calibration[calibration].ladder[ladder].pedestal_mean = f_math_mean(environment->calibration[calibration].ladder[ladder].
				pedestal, d_package_channels);
			environment->calibration[calibration].ladder[ladder].sigma_raw_mean = f_math_mean(environment->calibration[calibration].ladder[ladder].
				sigma_raw, d_package_channels);
			environment->calibration[calibration].ladder[ladder].sigma_mean = f_math_mean(environment->calibration[calibration].ladder[ladder].
				sigma, d_package_channels);
			environment->calibration[calibration].ladder[ladder].pedestal_rms = f_math_rms(environment->calibration[calibration].ladder[ladder].
				pedestal, d_package_channels, 0.01);
			environment->calibration[calibration].ladder[ladder].sigma_raw_rms = f_math_rms(environment->calibration[calibration].ladder[ladder].
				sigma_raw, d_package_channels, 0.01);
			environment->calibration[calibration].ladder[ladder].sigma_rms = f_math_rms(environment->calibration[calibration].ladder[ladder].
				sigma, d_package_channels, 0.01);
		}
}

int p_analyze_calibration_file_read_row(char *string, float *pedestal, float *sigma_raw, float *sigma) {
	char *begin_pointer = string, *end_pointer;
	float content[d_analyze_calibration_columns];
	int index = 0;
	while ((index < d_analyze_calibration_columns) && (end_pointer = strchr(begin_pointer, ','))) {
		*end_pointer = '\0';
		content[index++] = atof(begin_pointer);
		begin_pointer = (end_pointer+1);
	}
	if ((index < d_analyze_calibration_columns) && (strlen(begin_pointer) > 0))
		content[index++] = atof(begin_pointer);
	*pedestal = content[3];
	*sigma_raw = content[4];
	*sigma = content[5];
	return (int)content[0];
}

int p_analyze_calibration_file_read(struct s_analyze_environment *environment, FILE *stream, int calibration, int ladder) {
	char buffer[d_string_buffer_size];
	float pedestal = 0.0f, sigma_raw = 0.0f, sigma = 0.0f;
	int channel = 0, result = d_true;
	while (!feof(stream))
		if (fgets(buffer, d_string_buffer_size, stream) > 0) {
			channel = p_analyze_calibration_file_read_row(buffer, &pedestal, &sigma_raw, &sigma);
			if ((channel >= 0) && (channel < d_package_channels)) {
				environment->calibration[calibration].ladder[ladder].pedestal[channel] = pedestal;
				environment->calibration[calibration].ladder[ladder].sigma_raw[channel] = sigma_raw;
				environment->calibration[calibration].ladder[ladder].sigma[channel] = sigma;
			} else
				fprintf(stderr, "warning, channel %d has been readed (but channels have to be between 0 and %d)\n", channel,
						d_package_channels);
		}
	return result;
}

int p_analyze_calibration_file(struct s_analyze_environment *environment, const char *file, int calibration) {
	int result = d_false, ladder = 0;
	FILE *stream;
	char *next_pointer;
	if ((stream = fopen(file, "r"))) {
		if ((next_pointer = strrchr(file, '_')))
			ladder = atoi(next_pointer+1);
		if ((ladder >= 0) && (ladder < d_analyze_ladders))
			result = p_analyze_calibration_file_read(environment, stream, calibration, ladder);
		fclose(stream);
	}
	return result;
}

int p_analyze_calibration(struct s_analyze_environment *environment, const char *directory, int calibration) {
	DIR *stream;
	struct dirent *descriptor;
	int result = d_false;
	char next_directory[d_string_buffer_size], *next_pointer;
	if ((stream = opendir(directory))) {
		while ((descriptor = readdir(stream)))
			if (descriptor->d_name[0] != '.') {
				snprintf(next_directory, d_string_buffer_size, "%s/%s", directory, descriptor->d_name);
				p_analyze_calibration(environment, next_directory, calibration);
			}
		closedir(stream);
		result = d_true;
	} else if ((next_pointer = strrchr(directory, '.')) && (f_string_strcmp(next_pointer, ".cal") == 0))
		result = p_analyze_calibration_file(environment, directory, calibration);
	return result;
}

int f_analyze_calibration(struct s_analyze_environment *environment, const char *reference, const char *directory) {
	return ((p_analyze_calibration(environment, reference, 0)) && (p_analyze_calibration(environment, directory, 1)));
}

void f_analyze_export(struct s_analyze_environment *environment, const char *file, int *selected) {
	FILE *stream;
	int ladder, channel, calibration;
	if ((stream = fopen(file, "w"))) {
		fprintf(stream, "ladder%cchannel", d_analyze_csv_divisor);
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%cpedestal_%02d%csigma_raw_%02d%csigma_%02d", d_analyze_csv_divisor, calibration, d_analyze_csv_divisor, calibration,
					d_analyze_csv_divisor, calibration);
		fputc('\n', stream);
		for (ladder = 0; ladder < d_analyze_ladders; ++ladder)
			if (selected[ladder])
				for (channel = 0; channel < d_package_channels; ++channel) {
					fprintf(stream, "%d%c%d", ladder, d_analyze_csv_divisor, channel);
					for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
						fprintf(stream, "%c%.02f%c%.02f%c%.02f", d_analyze_csv_divisor,
								environment->calibration[calibration].ladder[ladder].pedestal[channel],
								d_analyze_csv_divisor, environment->calibration[calibration].ladder[ladder].sigma_raw[channel],
								d_analyze_csv_divisor, environment->calibration[calibration].ladder[ladder].sigma[channel]);
					fputc('\n', stream);
				}
		fclose(stream);
	}
}
