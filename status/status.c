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
#include "status.h"
struct s_status_environment environment;
struct s_chart_color temperature_colors[] = {
	{179,	204,	255},
	{180,	205,	255},
	{182,	206,	255},
	{184,	207,	255},
	{186,	208,	255},
	{255,	161,	72},
	{255,	137,	18},
	{255,	109,	0},
	{255,	51,	0},
	{255,	0,	0},
	{0,	0,	0}
};
int f_status_initialize(struct s_interface *supplied, const char *buiilder_path) {
	int result = d_false, index;
	if (f_interface_initialize(supplied, buiilder_path)) {
		gtk_window_set_default_size(supplied->window, d_status_window_width, d_status_window_height);
		if (g_signal_connect(G_OBJECT(supplied->window), "delete-event", G_CALLBACK(f_status_destroy), supplied) > 0) {
			gtk_widget_show_all(GTK_WIDGET(supplied->window));
			result = d_true;
		}
	}
	for (index = 0; index <= e_interface_chart_temperature_23; ++index) {
		strncpy(supplied->logic_charts[index].data.extension[0], "C", d_string_buffer_size);
		strncpy(supplied->logic_charts[index].data.extension[1], "C", d_string_buffer_size);
	}
	strncpy(supplied->logic_charts[e_interface_chart_temperature].data.description[0], "TFHs mean", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_temperature].data.description[1], "Power board", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_temperature].data.description[2], "ADC board", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_temperature].data.description[3], "FPGA board (BUS A)", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_temperature].data.description[4], "FPGA board (BUS B)", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_current].data.description[0], "Current on +3.4V", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_current].data.description[1], "Current on -3.3V", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_current].data.description[2], "Current on +5.7V", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_current].data.description[3], "Current on +12V", d_string_buffer_size);
	return result;
}

void f_status_destroy(GtkWidget *widget, struct s_interface *supplied) {
	if (environment.stream)
		fclose(environment.stream);
	exit(0);
}

void p_status_loop_fill_map_temperature(float kelvin, float *R, float *G, float *B) {
	float normal = (kelvin-d_status_temperature_min)/(d_status_temperature_max-d_status_temperature_min);
	int real_index = (int)(normal*(d_status_temperature_entries-1));
	if ((real_index > (d_status_temperature_entries-1)) || (real_index < 0))
		real_index = d_status_temperature_entries;
	*R = (((float)temperature_colors[real_index].R)/255.0f);
	*G = (((float)temperature_colors[real_index].G)/255.0f);
	*B = (((float)temperature_colors[real_index].B)/255.0f);
}

void p_status_loop_fill_map(struct s_interface *interface) {
	int index, real_index;
	for (index = 0, real_index = 0; real_index < d_trb_device_temperatures_size; ++index, real_index += 2) {
		f_chart_flush(&(interface->logic_charts[index]));
		f_chart_append_signal(&(interface->logic_charts[index]), 0, 5.0f, v_current_status.tfh_temperatures[real_index]);
		f_chart_append_signal(&(interface->logic_charts[index]), 1, 10.0f, v_current_status.tfh_temperatures[real_index+1]);
		p_status_loop_fill_map_temperature(v_current_status.tfh_temperatures[real_index]+d_status_temperature_kelvin,
				&(interface->logic_charts[index].data.color[0].R), &(interface->logic_charts[index].data.color[0].G),
				&(interface->logic_charts[index].data.color[0].B));
		p_status_loop_fill_map_temperature(v_current_status.tfh_temperatures[real_index+1]+d_status_temperature_kelvin,
				&(interface->logic_charts[index].data.color[1].R), &(interface->logic_charts[index].data.color[1].G),
				&(interface->logic_charts[index].data.color[1].B));
		interface->logic_charts[index].kind[0] = e_chart_kind_histogram_numeric;
		interface->logic_charts[index].kind[1] = e_chart_kind_histogram_numeric;
	}
	f_chart_flush(&(interface->logic_charts[e_interface_chart_temperature]));
	f_chart_flush(&(interface->logic_charts[e_interface_chart_current]));
	for (index = 0; index < environment.entries; ++index) {
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 0, index, environment.temperatures.tfh_mean[index]);
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 1, index, environment.temperatures.power_board[index]);
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 2, index, environment.temperatures.adc_board[index]);
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 3, index, environment.temperatures.fpga_board_busa[index]);
		f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 4, index, environment.temperatures.fpga_board_busb[index]);
	}
}

void p_status_loop_fill_values(void) {
	float tfh_mean = 0;
	int index, tfh_elements = 0;
	for (index = 0; index < d_trb_device_temperatures_size; ++index)
		if (v_current_status.tfh_temperatures[index] != d_status_temperature_wrong) {
			tfh_mean += v_current_status.tfh_temperatures[index];
			tfh_elements++;
		}
	if (tfh_elements > 1)
		tfh_mean /= (float)tfh_elements;
	environment.temperatures.tfh_mean[environment.entries] = tfh_mean;
	environment.temperatures.power_board[environment.entries] = v_current_status.temperatures[e_trb_device_temperatures_power];
	environment.temperatures.adc_board[environment.entries] = v_current_status.temperatures[e_trb_device_temperatures_adc];
	environment.temperatures.fpga_board_busa[environment.entries] = v_current_status.temperatures[e_trb_device_temperatures_fpga_A];
	environment.temperatures.fpga_board_busb[environment.entries] = v_current_status.temperatures[e_trb_device_temperatures_fpga_B];
	if (environment.entries >= (d_status_temperature_data-1)) {
		for (index = 1; index < d_status_temperature_data; ++index) {
			environment.temperatures.power_board[index-1] = environment.temperatures.power_board[index];
			environment.temperatures.adc_board[index-1] = environment.temperatures.adc_board[index];
			environment.temperatures.fpga_board_busa[index-1] = environment.temperatures.fpga_board_busa[index];
			environment.temperatures.fpga_board_busb[index-1] = environment.temperatures.fpga_board_busb[index];
		}
	} else
		environment.entries++;
}

int f_status_loop(struct s_interface *interface) {
	int result = d_true, index;
	char label_buffer[d_string_buffer_size], time_buffer[d_string_buffer_size];
	if (f_analyze_next(environment.stream, environment.code, &(environment.timestamp))) {
		p_status_loop_fill_values();
		p_status_loop_fill_map(interface);
		if (environment.timestamp > 0) {
			strftime(time_buffer, d_string_buffer_size, d_status_timestamp_format, localtime(&(environment.timestamp)));
			snprintf(label_buffer, d_string_buffer_size, "Last entry: %s", time_buffer);
			gtk_label_set_text(interface->labels[e_interface_label_update], label_buffer);
		}
		usleep(d_status_timeout);
	}
	for (index = 0; index < e_interface_chart_NULL; ++index)
		f_chart_redraw(&(interface->logic_charts[index]));
	return result;
}

int main (int argc, char *argv[]) {
	char buffer[d_string_buffer_size];
	struct s_interface main_interface;
	f_memory_init();
	if (argc == 3) {
		environment.code = (unsigned char)strtol(argv[2], NULL, 16);
		if ((environment.stream = fopen(argv[1], "r"))) {
			gtk_init(&argc, &argv);
			if (f_status_initialize(&main_interface, "UI/UI_main.glade")) {
				snprintf(buffer, d_string_buffer_size, "Magrathea status viewer (TRB %s)", argv[2]);
				gtk_window_set_title(main_interface.window, buffer);
				gtk_idle_add((GSourceFunc)f_status_loop, &main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "404 - file %s not found\n", argv[1]);
	} else
		fprintf(stderr, "usage:\n%s <file> <code>\n", argv[0]);
	f_memory_destroy();
	return 0;
}

