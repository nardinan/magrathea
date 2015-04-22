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
struct s_status_environment v_environment;
time_t v_starting_timestamp;
int v_frames;
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
int f_status_initialize(struct s_interface *supplied, const char *builder_path) {
	int result = d_false, index;
	if (f_interface_initialize(supplied, builder_path)) {
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
	strncpy(supplied->logic_charts[e_interface_chart_voltage].data.description[0], "BIAS A", d_string_buffer_size);
	strncpy(supplied->logic_charts[e_interface_chart_voltage].data.description[1], "BIAS B", d_string_buffer_size);
	return result;
}

void f_status_destroy(GtkWidget *widget, struct s_interface *supplied) {
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
	time_t first_timestamp;
	for (index = 0, real_index = 0; real_index < d_trb_device_temperatures_size; ++index, real_index += 2) {
		interface->logic_charts[index].data.line_size[0] = (interface->logic_charts[index].last_width/3.0);
		interface->logic_charts[index].data.line_size[1] = (interface->logic_charts[index].last_width/3.0);
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
	first_timestamp = v_environment.values[0].timestamp;
	f_chart_flush(&(interface->logic_charts[e_interface_chart_temperature]));
	f_chart_flush(&(interface->logic_charts[e_interface_chart_current]));
	f_chart_flush(&(interface->logic_charts[e_interface_chart_voltage]));
	for (index = 0; index < v_environment.entries; ++index) {
		real_index = (first_timestamp-v_environment.values[index].timestamp);
		if ((interface->logic_charts[e_interface_chart_temperature].axis_x.range[0] <= real_index) &&
				(interface->logic_charts[e_interface_chart_temperature].axis_x.range[1] >= real_index)) {
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 0, real_index, v_environment.values[index].tfh_mean);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 1, real_index, 
					v_environment.values[index].temperatures[e_trb_device_temperatures_power]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 2, real_index,
					v_environment.values[index].temperatures[e_trb_device_temperatures_adc]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 3, real_index,
					v_environment.values[index].temperatures[e_trb_device_temperatures_fpga_A]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_temperature]), 4, real_index,
					v_environment.values[index].temperatures[e_trb_device_temperatures_fpga_B]);
		}
		if ((interface->logic_charts[e_interface_chart_current].axis_x.range[0] <= real_index) &&
				(interface->logic_charts[e_interface_chart_current].axis_x.range[1] >= real_index)) {
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_current]), 0, real_index, 
					v_environment.values[index].currents[e_trb_device_currents_34]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_current]), 1, real_index, 
					v_environment.values[index].currents[e_trb_device_currents_33]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_current]), 2, real_index, 
					v_environment.values[index].currents[e_trb_device_currents_57]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_current]), 3, real_index, 
					v_environment.values[index].currents[e_trb_device_currents_12]);
		}
		if ((interface->logic_charts[e_interface_chart_voltage].axis_x.range[0] <= real_index) &&
				(interface->logic_charts[e_interface_chart_voltage].axis_x.range[1] >= real_index)) {
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_voltage]), 0, real_index,
					v_environment.values[index].voltages[e_trb_device_voltages_HV1]);
			f_chart_append_signal(&(interface->logic_charts[e_interface_chart_voltage]), 1, real_index,
					v_environment.values[index].voltages[e_trb_device_voltages_HV2]);
		}
	}
}

void p_status_loop_fill_values(void) {
	int index, tfh_elements;
	for (index = (d_status_bucket_size-1); index > 0; --index)
		memcpy(&(v_environment.values[index]), &(v_environment.values[index-1]), sizeof(struct s_status_environment_values));
	v_environment.values[0].tfh_mean = 0;
	for (index = 0, tfh_elements = 0; index < d_trb_device_temperatures_size; ++index)
		if (v_current_status.tfh_temperatures[index] != d_status_temperature_wrong) {
			v_environment.values[0].tfh_mean += v_current_status.tfh_temperatures[index];
			tfh_elements++;
		}
	if (tfh_elements > 1)
		v_environment.values[0].tfh_mean /= (float)tfh_elements;
	for (index = 0; index < e_trb_device_temperatures_null; ++index)
		v_environment.values[0].temperatures[index] = v_current_status.temperatures[index];
	for (index = 0; index < e_trb_device_currents_null; ++index)
		v_environment.values[0].currents[index] = v_current_status.currents[index];
	for (index = 0; index < e_trb_device_voltages_null; ++index)
		v_environment.values[0].voltages[index] = v_current_status.voltages[index];
	if (v_environment.entries < d_status_bucket_size)
		v_environment.entries++;
}

int f_status_loop(struct s_interface *interface) {
	time_t timeout = d_status_timeout;
	int result = d_true, index;
	char label_buffer[d_string_buffer_size], time_buffer[d_string_buffer_size];
	if (f_analyze_next(v_environment.stream, v_environment.code, &(v_environment.values[0].timestamp), v_starting_timestamp)) {
		p_status_loop_fill_values();
		if (v_environment.values[0].timestamp > 0) {
			strftime(time_buffer, d_string_buffer_size, d_status_timestamp_format, localtime(&(v_environment.values[0].timestamp)));
			snprintf(label_buffer, d_string_buffer_size, "last entry: %s", time_buffer);
			gtk_label_set_text(interface->labels[e_interface_label_update], label_buffer);
		}
		timeout = d_status_timeout_online;
	}
	if (v_frames == d_status_skip_frames) {
		p_status_loop_fill_map(interface);
		for (index = 0; index < e_interface_chart_NULL; ++index)
			f_chart_redraw(&(interface->logic_charts[index]));
		v_frames = 0;
	} else
		v_frames++;
	usleep(timeout);
	return result;
}

int main (int argc, char *argv[]) {
	char buffer[d_string_buffer_size];
	struct s_interface *main_interface = (struct s_interface *) malloc(sizeof(struct s_interface));
	int index;
	f_memory_init();
	if (argc >= 3) {
		v_environment.code = (unsigned char)strtol(argv[2], NULL, 16);
		if ((v_environment.stream = fopen(argv[1], "r"))) {
			for (index = 3; index < argc; ++index)
				if (f_string_strcmp(argv[index], "-l") == 0)
					fseek(v_environment.stream, 0, SEEK_END);
				else if ((f_string_strcmp(argv[index], "-bt") == 0) && ((index+1) < argc)) {
					v_starting_timestamp = atol(argv[index+1]);
					index++;
				}
			gtk_init(&argc, &argv);
			if (f_status_initialize(main_interface, "UI/UI_main.glade")) {
				snprintf(buffer, d_string_buffer_size, "Magrathea status viewer (TRB %s)", argv[2]);
				gtk_window_set_title(main_interface->window, buffer);
				gtk_idle_add((GSourceFunc)f_status_loop, main_interface);
				gtk_main();
			}
		} else
			fprintf(stderr, "404 - file %s not found\n", argv[1]);
	} else
		fprintf(stderr, "usage: %s <path> <trb#> {...}\n"
				"{-l: skip to last position}\n"
				"{-bt <timestamp>: show status after a date}\n", argv[0]);
	f_memory_destroy();
	return 0;
}

