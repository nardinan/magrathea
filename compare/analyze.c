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
	for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
		bad_channels = 0;
		for (channel = 0; channel < d_package_channels; ++channel)
			if (fabs(environment->pedestal_distance[ladder][channel]) >= d_analyze_pedestal_distance_max)
				bad_channels++;
		fprintf(stream, "ladder %02d [different channels %s%d\x1B[0m]", ladder, (bad_channels>0)?"\x1B[31m":"\x1B[32m", bad_channels);
		fprintf(stream, "\tPED ");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), 
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].pedestal_mean,
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].pedestal_rms);
		fputc(']', stream);
		fprintf(stream, "\tSRAW");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), 
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma_raw_mean,
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma_raw_rms);
		fputc(']', stream);
		fprintf(stream, "\tSIG ");
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%c %7.02f(+/-%5.02f) ", ((calibration==0)?'[':'~'), 
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma_mean,
					environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma_rms);
		fprintf(stream, "]\n");
		fflush(stream);
	}
}

void f_analyze_values(struct s_analyze_environment *environment) {
	int calibration, channel, ladder;
	float value;
	for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
		for (channel = 0; channel < d_package_channels; ++channel) {
			value = environment->calibration[0].ladder[environment->selected_trb][ladder].pedestal[channel];
			for (calibration = 1; calibration < d_analyze_calibrations; ++calibration)
				value -= (float)environment->calibration[calibration].ladder[environment->selected_trb][ladder].pedestal[channel];
			environment->pedestal_distance[ladder][channel] = value;
		}
	}
	for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
		f_calibrations_values(&(environment->calibration[calibration]), environment->selected_trb);
}

int f_analyze_calibration(struct s_analyze_environment *environment, const char *reference, const char *directory) {
	return ((f_calibrations(&(environment->calibration[0]), reference)) && (f_calibrations(&(environment->calibration[1]), directory)));
}

void f_analyze_export(struct s_analyze_environment *environment, const char *file, int *selected) {
	FILE *stream;
	int ladder, channel, calibration, channels_lower_five = 0, channels_bigger_five = 0;
	if ((stream = fopen(file, "w"))) {
		fprintf(stream, "ladder%cchannel", d_analyze_csv_divisor);
		for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
			fprintf(stream, "%cpedestal_%02d%csigma_raw_%02d%csigma_%02d", d_analyze_csv_divisor, calibration, d_analyze_csv_divisor, calibration,
					d_analyze_csv_divisor, calibration);
		fputc('\n', stream);
		for (ladder = 0; ladder < d_calibrations_ladders; ++ladder)
			if (selected[ladder]) {
				printf("ladder %d (channels with sigma > 5): ", ladder);
				for (channel = 0; channel < d_package_channels; ++channel) {
					if (environment->calibration[0].ladder[environment->selected_trb][ladder].sigma[channel] > 5) {
						printf("%d, ", channel);
						channels_bigger_five++;
					} else if (environment->calibration[0].ladder[environment->selected_trb][ladder].sigma[channel] >= 2)
						channels_lower_five++;
					fprintf(stream, "%d%c%d", ladder, d_analyze_csv_divisor, channel);
					for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
						fprintf(stream, "%c%.02f%c%.02f%c%.02f", d_analyze_csv_divisor,
								environment->calibration[calibration].ladder[environment->selected_trb][ladder].pedestal[channel],
								d_analyze_csv_divisor, environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma_raw[channel],
								d_analyze_csv_divisor, environment->calibration[calibration].ladder[environment->selected_trb][ladder].sigma[channel]);
					fputc('\n', stream);
				}
				putchar('\n');
			}
		printf("[sigma >= 2 && sigma <= 5]: %d\n[sigma > 5]: %d\n", channels_lower_five, channels_bigger_five);
		fclose(stream);
	}
}
