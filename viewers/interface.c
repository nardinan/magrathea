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
	"v_interface_chart_signal",
}, *v_interface_labels[] = {
	"v_interface_label_events",
	"v_interface_label_bad_events"
}, *v_interface_chart_labels[] = {
	"ADC_count",
	"Pedestal",
	"Sigma_raw",
	"Sigma",
	"ADC_pedestal",
	"ADC_pedestal_CN",
	"Signal"
}, *v_interface_chart_styles[] = {
	"styles/adc.keys",
	"styles/pedestal.keys",
	"styles/sigma_raw.keys",
	"styles/sigma.keys",
	"styles/adc_pedestal.keys",
	"styles/adc_pedestal_cn.keys",
	"styles/signal.keys",
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
			for (index = 0; index < e_interface_chart_NULL; index++) {
				d_assert(f_chart_new(&(result->logic_charts[index])));
				f_chart_style(&(result->logic_charts[index]), "styles/base_graph.keys");
				f_chart_style(&(result->logic_charts[index]), v_interface_chart_styles[index]);
				d_assert(result->charts[index] = GTK_ALIGNMENT(gtk_builder_get_object(result->interface, v_interface_charts[index])));
				gtk_container_add(GTK_CONTAINER(result->charts[index]), g_object_ref(result->logic_charts[index].plane));
			}
			//for (index = 0; index < e_interface_label_NULL; index++)
			//	d_assert(result->labels[index] = GTK_LABEL(gtk_builder_get_object(result->interface, v_interface_labels[index])));
		}
	}
	return result;
}

