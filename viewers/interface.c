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
#include "interface.h"
const char *v_interface_charts[] = {
	"v_interface_chart_adc",
	"v_interface_chart_pedestal",
	"v_interface_chart_sigma_raw",
	"v_interface_chart_sigma",
	"v_interface_chart_adc_pedestal",
	"v_interface_chart_adc_pedestal_cn",
	"v_interface_adc_0",
	"v_interface_adc_1",
	"v_interface_adc_2",
	"v_interface_adc_3",
	"v_interface_adc_4",
	"v_interface_adc_5",
	"v_interface_adc_6",
	"v_interface_adc_7",
	"v_interface_adc_8",
	"v_interface_adc_9",
	"v_interface_adc_10",
	"v_interface_adc_11",
	"v_interface_adc_12",
	"v_interface_adc_13",
	"v_interface_adc_14",
	"v_interface_adc_15",
	"v_interface_adc_16",
	"v_interface_adc_17",
	"v_interface_adc_18",
	"v_interface_adc_19",
	"v_interface_adc_20",
	"v_interface_adc_21",
	"v_interface_adc_22",
	"v_interface_adc_23",
	"v_interface_signal_0",
	"v_interface_signal_1",
	"v_interface_signal_2",
	"v_interface_signal_3",
	"v_interface_signal_4",
	"v_interface_signal_5",
	"v_interface_signal_6",
	"v_interface_signal_7",
	"v_interface_signal_8",
	"v_interface_signal_9",
	"v_interface_signal_10",
	"v_interface_signal_11",
	"v_interface_signal_12",
	"v_interface_signal_13",
	"v_interface_signal_14",
	"v_interface_signal_15",
	"v_interface_signal_16",
	"v_interface_signal_17",
	"v_interface_signal_18",
	"v_interface_signal_19",
	"v_interface_signal_20",
	"v_interface_signal_21",
	"v_interface_signal_22",
	"v_interface_signal_23"
}, *v_interface_labels[] = {
	"v_interface_label_events"
}, *v_interface_spins[] = {
	"v_interface_spin_ladder"
}, *v_interface_buttons[] = {
	"v_interface_button_dump",
	"v_interface_button_redo"
}, *v_interface_chart_labels[] = {
	"ADC_count",
	"Pedestal",
	"Sigma_raw",
	"Sigma",
	"ADC_pedestal",
	"ADC_pedestal_CN",
	"ADC_count_0",
	"ADC_count_1",
	"ADC_count_2",
	"ADC_count_3",
	"ADC_count_4",
	"ADC_count_5",
	"ADC_count_6",
	"ADC_count_7",
	"ADC_count_8",
	"ADC_count_9",
	"ADC_count_10",
	"ADC_count_11",
	"ADC_count_12",
	"ADC_count_13",
	"ADC_count_14",
	"ADC_count_15",
	"ADC_count_16",
	"ADC_count_17",
	"ADC_count_18",
	"ADC_count_19",
	"ADC_count_20",
	"ADC_count_21",
	"ADC_count_22",
	"ADC_count_23",
	"signal_0",
	"signal_1",
	"signal_2",
	"signal_3",
	"signal_4",
	"signal_5",
	"signal_6",
	"signal_7",
	"signal_8",
	"signal_9",
	"signal_10",
	"signal_11",
	"signal_12",
	"signal_13",
	"signal_14",
	"signal_15",
	"signal_16",
	"signal_17",
	"signal_18",
	"signal_19",
	"signal_20",
	"signal_21",
	"signal_22",
	"signal_23"
}, *v_interface_chart_styles[] = {
	"styles/adc.keys",
	"styles/pedestal.keys",
	"styles/sigma_raw.keys",
	"styles/sigma.keys",
	"styles/adc_pedestal.keys",
	"styles/adc_pedestal_cn.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys",
	"styles/adc_pedestal_cn_small.keys"
};
struct s_interface *f_interface_initialize(struct s_interface *supplied, const char *builder_path) {
	struct s_interface *result = supplied;
	int index;
	if (!result)
		if (!(result = (struct s_interface *)d_malloc(sizeof(struct s_interface))))
			d_die(d_error_malloc);
	result->destroy = d_false;
	if ((result->interface = gtk_builder_new())) {
		if ((gtk_builder_add_from_file(result->interface, builder_path, NULL))) {
			d_assert(result->window = GTK_WINDOW(gtk_builder_get_object(result->interface, "v_window")));
			for (index = 0; index < e_interface_chart_NULL; ++index) {
				d_assert(f_chart_new(&(result->logic_charts[index])));
				f_chart_style(&(result->logic_charts[index]), "styles/base_graph.keys");
				f_chart_style(&(result->logic_charts[index]), v_interface_chart_styles[index]);
				d_assert(result->charts[index] = GTK_ALIGNMENT(gtk_builder_get_object(result->interface, v_interface_charts[index])));
				gtk_container_add(GTK_CONTAINER(result->charts[index]), g_object_ref(result->logic_charts[index].plane));
			}
			for (index = 0; index < e_interface_label_NULL; ++index)
				d_assert(result->labels[index] = GTK_LABEL(gtk_builder_get_object(result->interface, v_interface_labels[index])));
			for (index = 0; index < e_interface_spin_NULL; ++index)
				d_assert(result->spins[index] = GTK_SPIN_BUTTON(gtk_builder_get_object(result->interface, v_interface_spins[index])));
			for (index = 0; index < e_interface_button_NULL; ++index)
				d_assert(result->buttons[index] = GTK_BUTTON(gtk_builder_get_object(result->interface, v_interface_buttons[index])));
		}
	}
	return result;
}

