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
FILE *v_stream;
unsigned char v_buffer[d_package_buffer_size];
size_t v_bytes;
int v_view_ladder;
int f_view_initialize(struct s_interface *supplied, const char *builder_path) {
	int result = d_false;
	if (f_interface_initialize(supplied, builder_path))
		if (g_signal_connect(G_OBJECT(supplied->window), "delete-event", G_CALLBACK(f_view_destroy), supplied) > 0) {
			gtk_widget_show_all(GTK_WIDGET(supplied->window));
			gtk_window_maximize(supplied->window);
			/* just hardcoded style == to be fixed with a dynamic style */
			supplied->logic_charts[e_interface_chart_adc].axis_x.show_negative = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_x.show_positive = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_x.show_grid = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_x.segments = 6;
			supplied->logic_charts[e_interface_chart_adc].axis_x.range[0] = 0.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_x.range[1] = 384.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_x.minimum_distance = 30.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_x.offset = 4.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_x.size = 1.0f;
			supplied->logic_charts[e_interface_chart_adc].border_x = 10;
			supplied->logic_charts[e_interface_chart_adc].axis_y.show_negative = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_y.show_positive = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_y.show_grid = d_true;
			supplied->logic_charts[e_interface_chart_adc].axis_y.segments = 20;
			supplied->logic_charts[e_interface_chart_adc].axis_y.range[0] = 0.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_y.range[1] = 1000.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_y.minimum_distance = 20.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_y.offset = 4.0f;
			supplied->logic_charts[e_interface_chart_adc].axis_y.size = 1.0f;
			supplied->logic_charts[e_interface_chart_adc].border_y = 10;
			supplied->logic_charts[e_interface_chart_adc].show_borders = d_true;
			supplied->logic_charts[e_interface_chart_adc].data.dot_size[0] = 0.0f;
			supplied->logic_charts[e_interface_chart_adc].data.line_size[0] = 1.5f;
			supplied->logic_charts[e_interface_chart_adc].data.color[0].R = 1.0f;
			supplied->logic_charts[e_interface_chart_adc].data.color[0].G = 0.0f;
			supplied->logic_charts[e_interface_chart_adc].data.color[0].B = 0.0f;
			result = d_true;
		}
	return result;
}

void f_view_destroy(GtkWidget *widget, struct s_interface *interface) {
	/* destroy everthing */
	if (v_stream)
		fclose(v_stream);
	exit(0);
}

int f_view_loop(struct s_interface *interface) {
	struct s_package package;
	int result = d_true, index, ladder;
	size_t readed;
	unsigned char *backup;
	if (v_stream) {
		if ((readed = fread(v_buffer+v_bytes, 1, d_package_buffer_size-v_bytes, v_stream)) > 0)
			v_bytes += readed;
		if ((backup = f_package_analyze(&package, v_buffer, v_bytes))) {
			v_bytes -= (backup-v_buffer);
			memmove(v_buffer, backup, v_bytes);
			if (package.complete)
				for (ladder = 0; ladder < d_package_ladders; ++ladder)
					if (package.data.values.raw.ladder[ladder] == v_view_ladder) {
						f_chart_flush(&(interface->logic_charts[e_interface_chart_adc]));
						for (index = 0; index < d_package_channels; ++index)
							f_chart_append_signal(&(interface->logic_charts[e_interface_chart_adc]), 0, index,
									package.data.values.raw.values[ladder][index]);
						break;
					}
		}
	}
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_redraw(&(interface->logic_charts[index]));
	return result;
}

int main (int argc, char *argv[]) {
	struct s_interface main_interface;
	if (argc == 3) {
		v_view_ladder = atoi(argv[2]);
		if ((v_stream = fopen(argv[1], "rb"))) {
			gtk_init(&argc, &argv);
			if (f_view_initialize(&main_interface, "UI/UI_main.glade")) {
				gtk_idle_add((GSourceFunc)f_view_loop, &main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "file not found: %s\n", argv[1]);
	} else
		fprintf(stderr, "usage: %s <path> <laddder#>\n", argv[0]);
	return 0;
}

