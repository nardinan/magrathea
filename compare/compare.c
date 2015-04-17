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
#include "compare.h"
struct s_analyze_environment environment;
int v_selected_ladder = 0;
int f_compare_initialize(struct s_interface *supplied, const char *builder_path) {
	int result = d_false;
	if (f_interface_initialize(supplied, builder_path)) {
		gtk_window_set_default_size(supplied->window, d_compare_window_width, d_compare_window_height);
		if (g_signal_connect(G_OBJECT(supplied->window), "delete-event", G_CALLBACK(f_compare_destroy), supplied) > 0) {
			gtk_spin_button_set_value(supplied->spins[e_interface_spin_ladder], v_selected_ladder);
			gtk_widget_show_all(GTK_WIDGET(supplied->window));
			result = d_true;
		}
	}
	return result;
}

void f_compare_destroy(GtkWidget *widget, struct s_interface *supplied) {
	exit(0);
}

int p_compare_loop_toggle(struct s_interface *interface, unsigned short int ladder, unsigned short int enable) {
	int result = d_false;
	if ((ladder >= 0) && (ladder < d_calibrations_ladders)) {
		if (enable)
			gtk_toggle_button_set_active(interface->switches[ladder], d_true);
		if (gtk_toggle_button_get_active(interface->switches[ladder])) {
			gtk_button_set_label(GTK_BUTTON(interface->switches[ladder]), "V");
			result = d_true;
		} else
			gtk_button_set_label(GTK_BUTTON(interface->switches[ladder]), "X");
	} else
		fprintf(stderr, "warning, ladder %d is out of range (0 - %d)\n", ladder, (d_calibrations_ladders-1));
	return result;
}

int f_compare_loop(struct s_interface *interface) {
	int result = d_true, index, calibration, ladder, channel, local_selection[d_calibrations_ladders];
	v_selected_ladder = -1;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(interface->radios[e_interface_radio_selected])))
		v_selected_ladder = gtk_spin_button_get_value_as_int(interface->spins[e_interface_spin_ladder]);
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_flush(&(interface->logic_charts[index]));
	for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
		local_selection[ladder] = p_compare_loop_toggle(interface, ladder, d_false);
		if (((v_selected_ladder < 0) && (local_selection[ladder]))|| (v_selected_ladder == ladder))
			for (channel = 0; channel < d_package_channels; ++channel)
				f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_distance]), 0,
						environment.pedestal_distance[ladder][channel]);
	}
	for (calibration = 0; calibration < d_analyze_calibrations; ++calibration) {
		for (ladder = 0; ladder < d_calibrations_ladders; ++ladder)
			if (((v_selected_ladder < 0) && (local_selection[ladder])) || (v_selected_ladder == ladder))
				for (channel = 0; channel < d_package_channels; ++channel) {
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_pedestal]), calibration,
							environment.calibration[calibration].ladder[environment.selected_trb][ladder].pedestal[channel]);
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_sigma_raw]), calibration,
							environment.calibration[calibration].ladder[environment.selected_trb][ladder].sigma_raw[channel]);
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_sigma]), calibration,
							environment.calibration[calibration].ladder[environment.selected_trb][ladder].sigma[channel]);
				}
	}
	if (v_selected_ladder >= 0)
		for (channel = 0; channel < d_package_channels; ++channel) {
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_raw_verify]), 0, channel,
					environment.calibration[0].ladder[environment.selected_trb][v_selected_ladder].sigma_raw[channel]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_verify]), 0, channel,
					environment.calibration[0].ladder[environment.selected_trb][v_selected_ladder].sigma[channel]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_raw_verify]), 1, channel,
					environment.calibration[1].ladder[environment.selected_trb][v_selected_ladder].sigma_raw[channel]*-1.0);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_sigma_verify]), 1, channel,
					environment.calibration[1].ladder[environment.selected_trb][v_selected_ladder].sigma[channel]*-1.0);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_pedestal_verify]), 0, channel,
					environment.pedestal_distance[v_selected_ladder][channel]);
		}
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_redraw(&(interface->logic_charts[index]));
	usleep(d_compare_timeout);
	return result;
}

int main (int argc, char *argv[]) {
	struct s_interface *main_interface = (struct s_interface *) malloc(sizeof(struct s_interface));
	char *current_pointer, *next_pointer, csv_file[d_string_buffer_size], default_calibration[d_string_buffer_size], buffer[d_string_buffer_size] = {'\0'};
	int index = 0, ladder, local_selection[d_calibrations_ladders];
	size_t length;
	f_memory_init();
	if (argc >= 2) {
		if ((current_pointer = strstr(argv[1], d_compare_postfix))) {
			length = f_string_strlen(argv[1]);
			while ((current_pointer < (length+argv[1])) && (!isdigit(*current_pointer)))
				current_pointer++;
			while ((current_pointer < (length+argv[1])) && (isdigit(*current_pointer))) {
				buffer[index++] = *current_pointer;
				current_pointer++;
			}
		}
		environment.selected_trb = atoi(buffer);
		snprintf(default_calibration, d_string_buffer_size, "%s%02d", d_compare_default, environment.selected_trb);
		if (argc >= 3)
			strncpy(default_calibration, argv[2], d_string_buffer_size);
		for (ladder = 0; ladder < d_calibrations_ladders; ++ladder)
			local_selection[ladder] = d_false;
		if (f_analyze_calibration(&environment, argv[1], default_calibration)) {
			snprintf(csv_file, d_string_buffer_size, "%s.csv", argv[1]);
			f_analyze_values(&environment);
			f_analyze_values_write(&environment, stdout);
			gtk_init(&argc, &argv);
			if (f_compare_initialize(main_interface, "UI/UI_main.glade")) {
				if (argc == 4) {
					current_pointer = argv[3];
					while ((next_pointer = strchr(current_pointer, ','))) {
						ladder = atoi(current_pointer);
						local_selection[ladder] = d_true;
						p_compare_loop_toggle(main_interface, ladder, d_true);
						current_pointer = (next_pointer+1);
					}
					if (f_string_strlen(current_pointer) > 0) {
						ladder = atoi(current_pointer);
						local_selection[ladder] = d_true;
						p_compare_loop_toggle(main_interface, ladder, d_true);
					}
				} else
					for (ladder = 0; ladder < d_calibrations_ladders; ++ladder) {
						local_selection[ladder] = d_true;
						p_compare_loop_toggle(main_interface, ladder, d_true);
					}
				f_analyze_export(&environment, csv_file, local_selection);
				gtk_idle_add((GSourceFunc)f_compare_loop, main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "one of these directories doesn't contains calibration files\n");
	} else
		fprintf(stderr, "usage:\n%s <calibration> {reference calibration} {list of masked ladders CSV}\n", argv[0]);
	f_memory_destroy();
	return 0;
}

