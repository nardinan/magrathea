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
#include "view.h"
struct s_view_environment environment;
int v_view_ladder, v_view_calibration_steps = d_view_calibration_steps, v_view_skip = 1;
long long v_view_index = 0;
int f_view_initialize(struct s_interface *supplied, const char *builder_path) {
	char buffer[d_string_buffer_size];
	int result = d_false;
	if (f_interface_initialize(supplied, builder_path)) {
		gtk_widget_set_sensitive(GTK_WIDGET(supplied->buttons[e_interface_button_dump]), FALSE);
		if (g_signal_connect(G_OBJECT(supplied->buttons[e_interface_button_dump]), "clicked", G_CALLBACK(f_view_dump), supplied) > 0) {
			gtk_spin_button_set_value(supplied->spins[e_interface_spin_ladder], v_view_ladder);
			snprintf(buffer, d_string_buffer_size, "%s (ladder %d)", d_view_window_title, v_view_ladder);
			gtk_window_set_title(supplied->window, buffer);
			gtk_window_set_default_size(supplied->window, d_view_window_width, d_view_window_height);
			if (g_signal_connect(G_OBJECT(supplied->window), "delete-event", G_CALLBACK(f_view_destroy), supplied) > 0) {
				gtk_widget_show_all(GTK_WIDGET(supplied->window));
				result = d_true;
			}
		}
	}
	return result;
}

void f_view_destroy(GtkWidget *widget, struct s_interface *interface) {
	if (environment.stream)
		fclose(environment.stream);
	exit(0);
}

void f_view_dump(GtkWidget *widget, struct s_interface *interface) {
	int index;
	if (environment.calibrated >= d_view_ladders)
		for (index = 0; index < d_view_ladders; ++index)
			p_view_loop_dump(interface, index);
}

void p_view_loop_dump(struct s_interface *interface, unsigned short int ladder) {
	char directory[PATH_MAX], filename[PATH_MAX];
	FILE *stream;
	int index, va, channel_on_va;
	if (environment.calibration[ladder].computed) {
		snprintf(directory, PATH_MAX, "%s_%s", environment.filename, d_view_calibration_folder);
		snprintf(filename, PATH_MAX, "%s/ladder_%02d.%s", directory, ladder, d_view_calibration_extension);
		if ((mkdir(directory, 0777) == 0) || (errno == EEXIST))
			if ((stream = fopen(filename, "w"))) {
				for (index = 0; index < d_package_channels; ++index) {
					va = index/d_package_channels_on_va;
					channel_on_va = index%d_package_channels_on_va;
					fprintf(stream, "%d, %d, %d, %.03f, %.03f, %.03f\n", index, va, channel_on_va,
							environment.calibration[ladder].pedestal[index], environment.calibration[ladder].sigma_raw[index],
							environment.calibration[ladder].sigma[index]);
				}
				fclose(stream);
			}
	}
}

void p_view_loop_analyze(struct s_interface *interface, unsigned short int ladder, unsigned short int *values) {
	int index;
	if (environment.calibration[ladder].package < environment.calibration[ladder].steps) {
		for (index = 0; index < d_package_channels; ++index)
			environment.calibration[ladder].bucket[environment.calibration[ladder].package][index] = values[index];
		environment.calibration[ladder].package++;
	} else if (!environment.calibration[ladder].computed)  {
		if (++(environment.calibrated) >= d_view_ladders)
			gtk_widget_set_sensitive(GTK_WIDGET(interface->buttons[e_interface_button_dump]), TRUE);
		f_analyze_pedestal(environment.calibration[ladder].bucket, environment.calibration[ladder].package, environment.calibration[ladder].pedestal);
		f_analyze_sigma_raw(environment.calibration[ladder].bucket, environment.calibration[ladder].package, environment.calibration[ladder].sigma_raw);
		f_analyze_sigma(environment.calibration[ladder].bucket, environment.calibration[ladder].package, d_view_calibration_sigma_k,
				environment.calibration[ladder].sigma_raw, environment.calibration[ladder].pedestal, environment.calibration[ladder].sigma);
		environment.calibration[ladder].computed = d_true;
	} else {
		for (index = 0; index < d_package_channels; ++index)
			environment.data[ladder].bucket[index] = values[index];
		f_analyze_adc_pedestal(environment.data[ladder].bucket, environment.calibration[ladder].pedestal, environment.data[ladder].adc_pedestal);
		f_analyze_adc_pedestal_cn(environment.data[ladder].bucket, d_view_calibration_sigma_k, environment.calibration[ladder].pedestal,
				environment.calibration[ladder].sigma, environment.data[ladder].adc_pedestal_cn);
	}
}

void p_view_loop_refresh(struct s_interface *interface, unsigned short int ladder, unsigned short int *values) {
	int index;
	if (ladder == v_view_ladder) {
		if (environment.calibration[v_view_ladder].computed) {
			if (!environment.calibration[v_view_ladder].drawed) {
				f_chart_flush(&(interface->logic_charts[e_interface_chart_pedestal]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma_raw]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma]));
				for (index = 0; index < d_package_channels; ++index) {
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_pedestal]), 0, index,
							environment.calibration[v_view_ladder].pedestal[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_raw]), 0, index,
							environment.calibration[v_view_ladder].sigma_raw[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma]), 0, index,
							environment.calibration[v_view_ladder].sigma[index]);
				}
				environment.calibration[v_view_ladder].drawed = d_true;
			} else {
				f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_pedestal]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_pedestal_cn]));
				for (index = 0; index < d_package_channels; ++index) {
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_pedestal]), 0, index,
							environment.data[v_view_ladder].adc_pedestal[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_pedestal_cn]), 0, index,
							environment.data[v_view_ladder].adc_pedestal_cn[index]);
				}
			}
		}
		f_chart_flush(&(interface->logic_charts[e_interface_chart_adc]));
		for (index = 0; index < d_package_channels; ++index)
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc]), 0, index, values[index]);
	}
	f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_0+ladder]));
	for (index = 0; index < d_package_channels; ++index)
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_0+ladder]), 0, index, values[index]);
}

int f_view_loop(struct s_interface *interface) {
	char buffer[d_string_buffer_size];
	unsigned char *backup;
	struct s_package package;
	int result = d_true, index, ladder, selected;
	ssize_t readed;
	v_view_index++;
	if (environment.stream) {
		selected = gtk_spin_button_get_value_as_int(interface->spins[e_interface_spin_ladder]);
		if ((selected != v_view_ladder) && (selected >= 0) && (selected < d_view_ladders)) {
			v_view_ladder = selected;
			environment.calibration[v_view_ladder].drawed = d_false;
			snprintf(buffer, d_string_buffer_size, "%s (ladder %d)", d_view_window_title, v_view_ladder);
			gtk_window_set_title(interface->window, buffer);
		}
		if ((readed = fread(environment.buffer+environment.bytes, 1, d_package_buffer_size-environment.bytes, environment.stream)) > 0)
			environment.bytes += readed;
		if ((backup = f_package_analyze(&package, environment.buffer, environment.bytes)))
			if (backup > environment.buffer) {
				environment.bytes -= (backup-environment.buffer);
				memmove(environment.buffer, backup, environment.bytes);
				if (package.complete)
					for (ladder = 0; ladder < d_package_ladders; ++ladder)
						if ((package.data.values.raw.ladder[ladder] >= 0) &&
								(package.data.values.raw.ladder[ladder] < d_view_ladders)) {
							environment.data[package.data.values.raw.ladder[ladder]].events++;
							p_view_loop_analyze(interface, package.data.values.raw.ladder[ladder],
									package.data.values.raw.values[ladder]);
							if ((v_view_index%v_view_skip) == 0)
								p_view_loop_refresh(interface, package.data.values.raw.ladder[ladder],
										package.data.values.raw.values[ladder]);
							if (package.data.values.raw.ladder[ladder] == v_view_ladder) {
								snprintf(buffer, d_string_buffer_size, "events: %zu", environment.data[v_view_ladder].events);
								gtk_label_set_text(interface->labels[e_interface_label_events], buffer);
							}
						}
			}
	}
	if ((v_view_index%v_view_skip) == 0)
		for (index = 0; index < e_interface_chart_NULL; ++index)
			f_chart_redraw(&(interface->logic_charts[index]));
	return result;
}

int main (int argc, char *argv[]) {
	struct s_interface main_interface;
	int index;
	f_memory_init();
	if (argc >= 3) {
		v_view_ladder = atoi(argv[2]);
		for (index = 0; index < d_view_ladders; ++index)
			environment.calibration[index].steps = v_view_calibration_steps;
		strncpy(environment.filename, argv[1], PATH_MAX);
		if ((environment.stream = fopen(environment.filename, "rb"))) {
			gtk_init(&argc, &argv);
			if (f_view_initialize(&main_interface, "UI/UI_main.glade")) {
				if (argc == 4)
					v_view_skip = atoi(argv[3]);
				gtk_idle_add((GSourceFunc)f_view_loop, &main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "file not found: %s\n", argv[1]);
	} else
		fprintf(stderr, "usage: %s <path> <laddder#>\n", argv[0]);
	f_memory_destroy();
	return 0;
}

