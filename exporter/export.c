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
#include "export.h"
void f_export_read(FILE *stream_out, FILE *stream_in, int trb, int ladder) {
	unsigned char buffer[d_package_buffer_size], *backup, packet_header[] = {0x90, 0xeb, 0x00, 0xa0}, packet_tail[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		      counter = 0;
	ssize_t readed, bytes = 0;
	struct s_package package;
	int selected_ladder = -1, channel, real_events = 0;
	while (d_true) {
		if ((readed = fread(buffer+bytes, 1, d_package_buffer_size-bytes, stream_in)) > 0)
			bytes += readed;
		if (((backup = f_package_analyze(&package, buffer, bytes))) && (backup > buffer)) {
			bytes -= (backup-buffer);
			memmove(buffer, backup, bytes);
			if ((package.complete) && (package.trb == v_package_trbs[trb].code))
				for (selected_ladder = 0; selected_ladder < d_package_ladders; ++selected_ladder)
					if ((ladder == -1) || (package.data.values.raw.ladder[selected_ladder] == ladder)) {
						packet_header[2] = (counter++);
						fwrite(packet_header, 1, sizeof(packet_header), stream_out);
						for (channel = 0; channel < d_package_channels_half; ++channel) {
							fwrite(&(package.data.values.raw.values[selected_ladder][channel]), 1, sizeof(unsigned short), stream_out);
							fwrite(&(package.data.values.raw.values[selected_ladder][channel+d_package_channels_half]), 1,
									sizeof(unsigned short), stream_out);
						}
						fwrite(packet_tail, 1, sizeof(packet_tail), stream_out);
						fprintf(stdout, "\r[event 0x%02x]", packet_header[2]);
						fflush(stdout);
						real_events++;
					}
		} else if (readed <= 0)
			break;
	}
	fprintf(stdout, "\n[%d events have been written]\n", real_events);
}

int main (int argc, char *argv[]) {
	FILE *stream_in, *stream_out;
	int trb, ladder = -1;
	char buffer[PATH_MAX];
	if (argc >= 3) {
		trb = atoi(argv[2]);
		if (argc >= 4)
			ladder = atoi(argv[3]);
		if ((trb >= 0) && (trb < d_trb_device_boards)) {
			if ((ladder >= -1) && (ladder < d_trb_device_ladders)) {
				if ((stream_in = fopen(argv[1], "rb"))) {
					if (ladder >= 0)
						snprintf(buffer, PATH_MAX, "%s.trb%02d.lad%02d%s", argv[1], trb, ladder, d_export_postfix);
					else
						snprintf(buffer, PATH_MAX, "%s.trb%02d%s", argv[1], trb, d_export_postfix);
					if ((stream_out = fopen(buffer, "wb"))) {
						f_export_read(stream_out, stream_in, trb, ladder);
						fclose(stream_out);
					}
					fclose(stream_in);
				} else
					fprintf(stderr, "unable to find file %s\n", argv[1]);
			} else
				fprintf(stderr, "%s LADDER doesn't exists\n", argv[3]);
		} else
			fprintf(stderr, "%s TRB doesn't exits\n", argv[2]);
	} else
		fprintf(stderr, "usage: %s <file> <trb#> {ladder#}\n", argv[0]);
	return 0;
}
