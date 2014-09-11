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

int f_compare_loop(struct s_interface *interface) {
	int result = d_true, index, calibration, ladder, channel;
	v_selected_ladder = -1;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(interface->radios[e_interface_radio_selected])))
		v_selected_ladder = gtk_spin_button_get_value_as_int(interface->spins[e_interface_spin_ladder]);
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_flush(&(interface->logic_charts[index]));
	for (calibration = 0; calibration < d_analyze_calibrations; ++calibration)
		for (ladder = 0; ladder < d_analyze_ladders; ++ladder)
			if ((v_selected_ladder < 0) || (v_selected_ladder == ladder))
				for (channel = 0; channel < d_package_channels; ++channel) {
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_pedestal]), calibration,
							environment.calibration[calibration].ladder[ladder].pedestal[channel]);
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_sigma_raw]), calibration,
							environment.calibration[calibration].ladder[ladder].sigma_raw[channel]);
					f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_sigma]), calibration,
							environment.calibration[calibration].ladder[ladder].sigma[channel]);
				}
	for (ladder = 0; ladder < d_analyze_ladders; ++ladder)
		if ((v_selected_ladder < 0) || (v_selected_ladder == ladder)) {
			for (channel = 0; channel < d_package_channels; ++channel)
				f_chart_append_histogram(&(interface->logic_charts[e_interface_chart_distance]), 0,
						environment.pedestal_distance[ladder][channel]);
		}
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_redraw(&(interface->logic_charts[index]));
	return result;
}

int main (int argc, char *argv[]) {
	struct s_interface *main_interface = (struct s_interface *) malloc(sizeof(struct s_interface));
	f_memory_init();
	if (argc >= 3) {
		if (f_analyze_calibration(&environment, argv[1], argv[2])) {
			f_analyze_values(&environment);
			f_analyze_values_write(&environment, stdout);
			gtk_init(&argc, &argv);
			if (f_compare_initialize(main_interface, "UI/UI_main.glade")) {
				gtk_idle_add((GSourceFunc)f_compare_loop, main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "one of these directories doesn't contains calibration files\n");
	} else
		fprintf(stderr, "usage:\n%s <reference calibration> <calibration>\n", argv[0]);
	f_memory_destroy();
	return 0;
}

