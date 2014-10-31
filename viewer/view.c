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
int v_view_ladder, v_view_trb, v_view_calibration_steps = d_view_calibration_steps, v_view_skip_frames = 1, v_view_pause = d_false,
    v_view_label_refresh = d_true, v_frames, v_flags = 0;
long long v_starting_time;
void f_view_action_dump(GtkWidget *widget, struct s_interface *interface) {
	if (environment.data.calibrated >= d_analyze_ladders)
		f_calibrations_export(&(environment.data.computed_calibrations), environment.filename, v_view_trb);
}

void f_view_action_redo(GtkWidget *widget, struct s_interface *interface) {
	int ladder;
	for (ladder = 0; ladder < d_analyze_ladders; ++ladder) {
		environment.data.calibration[ladder].package = 0;
		environment.data.calibration[ladder].computed = d_false;
		environment.data.calibration[ladder].drawed = d_false;
	}
	environment.data.calibrated = d_false;
	f_chart_flush(&(interface->logic_charts[e_interface_chart_pedestal]));
	f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma_raw]));
	f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma]));
	gtk_widget_set_sensitive(GTK_WIDGET(interface->buttons[e_interface_button_dump]), FALSE);
}

void f_view_action_last(GtkWidget *widget, struct s_interface *supplied) {
	if (environment.stream)
		fseek(environment.stream, 0, SEEK_END);
	environment.bytes = 0;
}

int f_view_action_press(GtkWidget *widget, GdkEventKey *event, struct s_interface *supplied) {
	switch (event->keyval) {
		case 32:	/* space button */
			v_view_pause = !v_view_pause;
			v_view_label_refresh = d_true;
			break;
		case 82:	/* r button */
		case 114:	/* R button */
			v_frames = v_view_skip_frames;
	}
	return d_false;
}

int f_view_initialize(struct s_interface *supplied, const char *builder_path) {
	int result = d_false;
	if (f_interface_initialize(supplied, builder_path)) {
		gtk_widget_set_sensitive(GTK_WIDGET(supplied->buttons[e_interface_button_dump]), FALSE);
		if ((g_signal_connect(G_OBJECT(supplied->buttons[e_interface_button_dump]), "clicked", G_CALLBACK(f_view_action_dump), supplied) > 0) &&
				(g_signal_connect(G_OBJECT(supplied->buttons[e_interface_button_redo]), "clicked",
						  G_CALLBACK(f_view_action_redo), supplied) > 0) &&
				(g_signal_connect(G_OBJECT(supplied->buttons[e_interface_button_last]), "clicked",
						  G_CALLBACK(f_view_action_last), supplied) > 0)) {
			gtk_spin_button_set_value(supplied->spins[e_interface_spin_ladder], v_view_ladder);
			gtk_spin_button_set_value(supplied->spins[e_interface_spin_delay], 0);
			gtk_window_set_default_size(supplied->window, d_view_window_width, d_view_window_height);
			if ((g_signal_connect(G_OBJECT(supplied->window), "delete-event", G_CALLBACK(f_view_destroy), supplied) > 0) &&
					(g_signal_connect(G_OBJECT(supplied->window), "key-press-event", G_CALLBACK(f_view_action_press), supplied) > 0)) {
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

void p_view_loop_analyze(struct s_interface *interface, unsigned short int ladder, unsigned short int *values) {
	int result = f_analyze_data(&(environment.data), ladder, values);
	char destination[PATH_MAX];
	if (environment.data.calibrated >= d_analyze_ladders) {
		if ((v_flags&e_view_action_exports_clusters) == e_view_action_exports_clusters) {
			snprintf(destination, PATH_MAX, "%s_clusters_TRB%02d.csv", environment.filename, v_view_trb);
			f_clusters_save(&(environment.data.data[ladder].compressed_event), environment.data.counters[ladder].data_events, ladder,
					destination);
		}
		gtk_widget_set_sensitive(GTK_WIDGET(interface->buttons[e_interface_button_dump]), TRUE);
		if (result)
			if ((v_flags&e_view_action_exports_calibrations) == e_view_action_exports_calibrations) {
				f_view_action_dump(NULL, interface);
				if ((v_flags&e_view_action_close_after_calibrations) == e_view_action_close_after_calibrations)
					f_view_destroy(NULL, interface);
			}
	}
}

void p_view_loop_append_signals(struct s_interface *interface, unsigned short int ladder) {
	int index, current_ladder, current_cluster, maximum;
	float cluster_signal;
	if (ladder == v_view_ladder) {
		if (environment.data.calibration[v_view_ladder].computed) {
			if (!environment.data.calibration[v_view_ladder].drawed) {
				f_chart_flush(&(interface->logic_charts[e_interface_chart_pedestal]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma_raw]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_sigma]));
				for (index = 0; index < d_package_channels; ++index) {
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_pedestal]), 0, index,
							environment.data.computed_calibrations.ladder[v_view_ladder].pedestal[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_raw]), 1, index,
							environment.data.computed_calibrations.ladder[v_view_ladder].sigma_raw[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_raw]), 0, index,
							-(((float)environment.data.computed_calibrations.ladder[v_view_ladder].flags[index])/10.0));
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma]), 0, index,
							environment.data.computed_calibrations.ladder[v_view_ladder].sigma[index]);
				}
				interface->logic_charts[e_interface_chart_sigma_raw].kind[0] = e_chart_kind_histogram;
				environment.data.calibration[v_view_ladder].drawed = d_true;
			} else {
				f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_pedestal]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_pedestal_cn]));
				for (index = 0; index < d_package_channels; ++index) {
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_pedestal]), 0, index,
							environment.data.data[v_view_ladder].adc_pedestal[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_pedestal_cn]), 0, index,
							environment.data.data[v_view_ladder].adc_pedestal_cn[index]);
				}
			}
		}
		f_chart_flush(&(interface->logic_charts[e_interface_chart_adc]));
		for (index = 0; index < d_package_channels; ++index)
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc]), 0, index, environment.data.data[v_view_ladder].bucket[index]);
		for (current_ladder = 0; current_ladder < d_analyze_ladders; ++current_ladder)
			if (environment.data.data[current_ladder].new_bucket) {
				f_chart_flush(&(interface->logic_charts[e_interface_chart_adc_0+current_ladder]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_signal_0+current_ladder]));
				f_chart_flush(&(interface->logic_charts[e_interface_chart_occupancy_0+current_ladder]));
				for (index = 0, maximum = 1; index < d_package_channels; ++index) {
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc_0+current_ladder]), 0, index,
							environment.data.data[current_ladder].bucket[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_signal_0+current_ladder]), 0, index,
							environment.data.data[current_ladder].adc_pedestal_cn[index]);
					f_chart_append_signal(&(interface->logic_charts[e_interface_chart_occupancy_0+current_ladder]), 0, index,
							(environment.data.data[current_ladder].occupancy[index]));
					if (environment.data.data[current_ladder].occupancy[index] > maximum)
						maximum = environment.data.data[current_ladder].occupancy[index];
				}
				interface->logic_charts[e_interface_chart_occupancy_0+current_ladder].axis_y.range[1] = (maximum+1);
				environment.data.data[current_ladder].new_bucket = d_false;
				for (current_cluster = 0; current_cluster < environment.data.data[current_ladder].compressed_event.clusters;
						++current_cluster) {
					for (index = 0, cluster_signal = 0; index <
							environment.data.data[current_ladder].compressed_event.values[current_cluster].strips; ++index)
						cluster_signal += environment.data.data[current_ladder].compressed_event.values[current_cluster].values[index];
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_clusters_distribution]), 0, cluster_signal);
				}
			}
	}
}

void p_view_loop_show_signals(struct s_interface *interface) {
	int index;
	switch (gtk_notebook_get_current_page(interface->notebook)) {
		case 0: /* ADC */
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_adc]));
			break;
		case 1: /* calibration */
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_pedestal]));
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_sigma_raw]));
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_sigma]));
			break;
		case 2: /* signal */
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_adc_pedestal]));
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_adc_pedestal_cn]));
			break;
		case 3:
			switch(gtk_notebook_get_current_page(interface->quick_notebook)) {
				case 0: /* ADCs */
					for (index = e_interface_chart_adc_0; index <= e_interface_chart_adc_23; ++index)
						f_chart_redraw(&(interface->logic_charts[index]));
					break;
				case 1: /* signals */
					for (index = e_interface_chart_signal_0; index <= e_interface_chart_signal_23; ++index)
						f_chart_redraw(&(interface->logic_charts[index]));
					break;
				case 2: /* occupancy */
					for (index = e_interface_chart_occupancy_0; index <= e_interface_chart_occupancy_23; ++index)
						f_chart_redraw(&(interface->logic_charts[index]));
			}
			break;
		case 4: /* clusters */
			f_chart_redraw(&(interface->logic_charts[e_interface_chart_clusters_distribution]));
	}
}

void p_view_loop_read_raw(int delay) {
	struct timeval current_timeval;
	long long current_time;
	size_t readed;
	gettimeofday(&current_timeval, NULL);
	current_time = ((1000000l*((long long)current_timeval.tv_sec))+current_timeval.tv_usec);
	if (((current_time-v_starting_time) > delay) && (!v_view_pause)) {
		v_starting_time = current_time;
		if ((readed = fread(environment.buffer+environment.bytes, 1, d_package_buffer_size-environment.bytes, environment.stream)) > 0)
			environment.bytes += readed;
	}
}

void p_view_loop_read_process(struct s_interface *interface, struct s_package *package) {
	int ladder;
	if ((package->complete) && (package->trb == v_package_trbs[v_view_trb].code))
		switch (package->data.kind) {
			case d_package_raw_workmode:
				v_analyze_adc_pedestal = d_true;
				v_analyze_adc_pedestal_cn = d_true;
				for (ladder = 0; ladder < d_package_ladders; ++ladder)
					if ((package->data.values.raw.ladder[ladder] >= 0) && (package->data.values.raw.ladder[ladder] <
								d_analyze_ladders)) {
						environment.data.counters[package->data.values.raw.ladder[ladder]].events++;
						p_view_loop_analyze(interface, package->data.values.raw.ladder[ladder],
								package->data.values.raw.values[ladder]);
						if (package->data.values.raw.ladder[ladder] == v_view_ladder)
							v_view_label_refresh = d_true;
					}
				break;
			case d_package_nrm_workmode:
				v_analyze_adc_pedestal = d_false;
				v_analyze_adc_pedestal_cn = d_false;
				for (ladder = 0; ladder < d_trb_device_ladders; ++ladder) {
					environment.data.calibration[ladder].computed = d_true;
					environment.data.calibration[ladder].package = environment.data.calibration[ladder].steps;
					environment.data.counters[ladder].events++;
					p_view_loop_analyze(interface, ladder, package->data.values.nrm.ladders_data[ladder].values);
					v_view_label_refresh = d_true;
				}
		}
}

int p_view_loop_read(struct s_interface *interface, int delay) {
	struct s_package package;
	unsigned char *backup;
	int result = d_false;
	p_view_loop_read_raw(delay);
	if ((backup = f_package_analyze(&package, environment.buffer, environment.bytes))) {
		result = d_true;
		if (backup > environment.buffer) {
			environment.bytes -= (backup-environment.buffer);
			memmove(environment.buffer, backup, environment.bytes);
			p_view_loop_read_process(interface, &package);
		}

	}
	p_view_loop_append_signals(interface, v_view_ladder);
	return result;
}

int f_view_loop(struct s_interface *interface) {
	int result = d_true, index, selected_ladder = 0, selected_delay = 0, charts_swap = d_false;
	char buffer[d_string_buffer_size];
	time_t timeout = d_view_timeout;
	if (environment.stream) {
		selected_ladder = gtk_spin_button_get_value_as_int(interface->spins[e_interface_spin_ladder]);
		selected_delay = gtk_spin_button_get_value_as_int(interface->spins[e_interface_spin_delay]);
		if (selected_ladder != v_view_ladder) { /* change visible ladder */
			charts_swap = d_true;
			v_view_label_refresh = d_true;
			v_view_ladder = selected_ladder;
			for (index = 0; index <= e_interface_chart_adc_pedestal_cn; index++)
				f_chart_flush(&(interface->logic_charts[index]));
			environment.data.calibration[v_view_ladder].drawed = d_false;
		}
		if (p_view_loop_read(interface, selected_delay))
			timeout = d_view_timeout_online;
	}
	if (v_view_label_refresh) {
		if (v_view_pause)
			strncpy(buffer, "[events]: pause", d_string_buffer_size);
		else
			snprintf(buffer, d_string_buffer_size, "[events]: %zu", environment.data.counters[v_view_ladder].events);
		gtk_label_set_text(interface->labels[e_interface_label_events], buffer);
		v_view_label_refresh = d_false;
	}
	if ((charts_swap) || (v_frames >= v_view_skip_frames)) {
		p_view_loop_show_signals(interface);
		v_frames = 0;
	} else
		v_frames++;
	usleep(timeout);
	return result;
}

int f_view_load_channel(const char *filename) {
	FILE *stream;
	int trb, ladder, channel, result = d_false;
	if ((stream = fopen(filename, "r"))) {
		result = d_true;
		while ((fscanf(stream, "%d %d %d\n", &trb, &ladder, &channel) == 3)) {
			if ((trb >= 0) && (trb <= d_trb_device_boards) && (trb == v_view_trb))
				if ((ladder >= 0) && (ladder <= d_trb_device_ladders))
					if ((channel >= 0) && (channel <= d_package_channels))
						environment.data.computed_calibrations.ladder[ladder].flags[channel] = e_stk_math_flag_bad;
		}
		fclose(stream);
	}
	return result;
}

int main (int argc, char *argv[]) {
	char buffer[d_string_buffer_size];
	struct s_interface *main_interface = (struct s_interface *) malloc(sizeof(struct s_interface));
	int index, ladder;
	f_memory_init();
	if (argc >= 5) {
		v_view_ladder = atoi(argv[3]);
		v_view_trb = atoi(argv[2]);
		if ((v_view_skip_frames = atoi(argv[4])) < 0)
			v_view_skip_frames = 0;
		if ((v_view_ladder >= 0) && (v_view_ladder < d_analyze_ladders)) {
			if ((v_view_trb >= 0) && (v_view_trb < d_trb_device_boards)) {
				for (index = 0; index < d_analyze_ladders; ++index)
					environment.data.calibration[index].steps = v_view_calibration_steps;
				strncpy(environment.filename, argv[1], PATH_MAX);
				if ((environment.stream = fopen(environment.filename, "rb"))) {
					for (index = 5; index < argc;) {
						if (f_string_strcmp(argv[index], "-l") == 0)
							fseek(environment.stream, 0, SEEK_END);
						else if (f_string_strcmp(argv[index], "-x") == 0)
							v_flags |= e_view_action_exports_calibrations;
						else if (f_string_strcmp(argv[index], "-k") == 0)
							v_flags |= e_view_action_close_after_calibrations;
						else if (f_string_strcmp(argv[index], "-e") == 0)
							v_flags |= e_view_action_exports_clusters;
						else if ((f_string_strcmp(argv[index], "-c") == 0) && ((index+1) < argc)) {
							if (f_calibrations(&(environment.data.computed_calibrations), argv[index+1])) {
								environment.data.calibrated = d_analyze_ladders;
								for (ladder = 0; ladder < d_analyze_ladders; ++ladder) {
									environment.data.calibration[ladder].computed = d_true;
									environment.data.calibration[ladder].package =
										environment.data.calibration[ladder].steps;
								}
							} else
								fprintf(stderr, "404 - calibration folder not found %s\n", argv[index+1]);
							index++;
						} else if ((f_string_strcmp(argv[index], "-m") == 0) && ((index+1) < argc)) {
							if (!f_view_load_channel(argv[index+1]))
								fprintf(stderr, "%s has a wrong file format\n", argv[index+1]);
							index++;
						} else
							fprintf(stderr, "wrong parameter '%s'\n", argv[index]);
						index++;
					}
					gtk_init(&argc, &argv);
					if (f_view_initialize(main_interface, "UI/UI_main.glade")) {
						snprintf(buffer, d_string_buffer_size, "Magrathea event viewer (TRB %d| stream %s)", v_view_trb, argv[1]);
						gtk_window_set_title(main_interface->window, buffer);
						gtk_idle_add((GSourceFunc)f_view_loop, main_interface);
						gtk_main();
					}
				} else
					fprintf(stderr, "file not found: %s\n", argv[1]);
			} else
				fprintf(stderr, "%s TRB doesn't exists\n", argv[2]);
		} else
			fprintf(stderr, "%s ladder doesn't exists\n", argv[3]);
	} else
		fprintf(stderr, "usage: %s <path> <trb#> <laddder#> <frequecy of refresh> {...}\n"
				"\t{-l: skip to last position}\n"
				"\t{-x: automatically export calibrations when ready}\n"
				"\t{-e: automatically export clusters when ready}\n"
				"\t{-k: exit after calibration}\n"
				"\t{-c <folder>: load an external TRB calibration}\n"
				"\t{-m <file>: file with bad channels}\n", argv[0]);
	f_memory_destroy();
	return 0;
}

