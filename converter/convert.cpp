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
#include "convert.h"
int tree_adc[d_package_ladders][d_package_channels];
struct s_convert_environment *f_convert_init(struct s_convert_environment *supplied, const char *prefix) {
	char directory[PATH_MAX], postfix[d_string_buffer_size], path[PATH_MAX], h_name[d_string_buffer_size], t_name[d_string_buffer_size];
	struct s_convert_environment *result = supplied;
	int ladder, channel;
	if (!result)
		if (!(result = (struct s_convert_environment *) d_malloc(sizeof(struct s_convert_environment))))
			d_die(d_error_malloc);
	snprintf(directory, PATH_MAX, "%s_%s", prefix, d_convert_directory);
	if ((mkdir(directory, 0777) == 0) || (errno == EEXIST))
		for (ladder = 0; ladder < d_package_ladders; ++ladder) {
			snprintf(postfix, d_string_buffer_size, "ladder_%d", ladder);
			snprintf(path, PATH_MAX, "%s/%s.root", directory, postfix);
			if ((result->stream[ladder] = new TFile(path, "RECREATE"))) {
				result->structure[ladder] = new TTree("events", "events from TRB");
				for (channel = 0; channel < d_package_channels; ++channel) {
					snprintf(h_name, d_string_buffer_size, "t_Ch%d", channel);
					snprintf(t_name, d_string_buffer_size, "adc%d/I", channel);
					result->structure[ladder]->Branch(h_name, &(tree_adc[ladder][channel]), t_name);
				}
			} else
				d_die(d_error_malloc);
		}
	return result;
}

void f_convert_destroy(struct s_convert_environment *supplied) {
	int ladder;
	for (ladder = 0; ladder < d_package_ladders; ++ladder) {
		supplied->stream[ladder]->Write();
		supplied->stream[ladder]->Close();
		delete(supplied->stream[ladder]);
	}
}

int f_convert_insert(struct s_convert_environment *environment, struct s_package *package) {
	int ladder, selected_ladder, channel, result = d_true;
	for (ladder = 0; ladder < d_package_ladders; ++ladder)
		if ((selected_ladder = package->data.values.raw.ladder[ladder]) < d_package_ladders) {
			for (channel = 0; channel < d_package_channels; ++channel)
				tree_adc[selected_ladder][channel] = package->data.values.raw.values[ladder][channel];
			environment->structure[selected_ladder]->Fill();
		}
	return result;
}

int f_convert_read(const char *prefix, FILE *stream) {
	unsigned char buffer[d_package_buffer_size], *backup;
	ssize_t readed, bytes = 0;
	struct s_package package;
	struct s_convert_environment environment;
	int result = d_true;
	if (f_convert_init(&environment, prefix)) {
		while ((readed = fread(buffer+bytes, 1, d_package_buffer_size-bytes, stream)) > 0) {
			bytes += readed;
			if ((backup = f_package_analyze(&package, buffer, bytes)))
				if (backup > buffer) {
					bytes -= (backup-buffer);
					memmove(buffer, backup, bytes);
					if (package.complete)
						f_convert_insert(&environment, &package);
				}
		}
		f_convert_destroy(&environment);
	}
	return result;
}

int main (int argc, char *argv[]) {
	FILE *stream;
	if (argc == 2) {
		if ((stream = fopen(argv[1], "rb"))) {
			f_convert_read(argv[1], stream);
			fclose(stream);
		} else
			fprintf(stderr, "unable to find file %s\n", argv[1]);
	} else
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
	return 0;
}
