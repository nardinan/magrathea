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
	"v_temperature_map_ladder_0",
	"v_temperature_map_ladder_1",
	"v_temperature_map_ladder_2",
	"v_temperature_map_ladder_3",
	"v_temperature_map_ladder_4",
	"v_temperature_map_ladder_5",
	"v_temperature_map_ladder_6",
	"v_temperature_map_ladder_7",
	"v_temperature_map_ladder_8",
	"v_temperature_map_ladder_9",
	"v_temperature_map_ladder_10",
	"v_temperature_map_ladder_11",
	"v_temperature_map_ladder_12",
	"v_temperature_map_ladder_13",
	"v_temperature_map_ladder_14",
	"v_temperature_map_ladder_15",
	"v_temperature_map_ladder_16",
	"v_temperature_map_ladder_17",
	"v_temperature_map_ladder_18",
	"v_temperature_map_ladder_19",
	"v_temperature_map_ladder_20",
	"v_temperature_map_ladder_21",
	"v_temperature_map_ladder_22",
	"v_temperature_map_ladder_23",
	"v_current_chart_alignment",
	"v_temperature_chart_alignment"
}, *v_interface_labels[] = {
	"v_update_label"
}, *v_interface_chart_labels[] = {
	"temperature_status_0",
	"temperature_status_1",
	"temperature_status_2",
	"temperature_status_3",
	"temperature_status_4",
	"temperature_status_5",
	"temperature_status_6",
	"temperature_status_7",
	"temperature_status_8",
	"temperature_status_9",
	"temperature_status_10",
	"temperature_status_11",
	"temperature_status_12",
	"temperature_status_13",
	"temperature_status_14",
	"temperature_status_15",
	"temperature_status_16",
	"temperature_status_17",
	"temperature_status_18",
	"temperature_status_19",
	"temperature_status_20",
	"temperature_status_21",
	"temperature_status_22",
	"temperature_status_23",
	"currents",
	"temperatures"
}, *v_interface_chart_styles[] = {
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/temperature_small.keys",
	"styles/currents.keys",
	"styles/temperatures.keys"
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
				d_assert(f_chart_hook_interface(&(result->logic_charts[index]), "../components/UI/UI_scale_config.glade"));
				f_chart_style(&(result->logic_charts[index]), "styles/base_graph.keys");
				f_chart_style(&(result->logic_charts[index]), v_interface_chart_styles[index]);
				d_assert(result->charts[index] = GTK_ALIGNMENT(gtk_builder_get_object(result->interface, v_interface_charts[index])));
				gtk_container_add(GTK_CONTAINER(result->charts[index]), g_object_ref(result->logic_charts[index].plane));
			}
			for (index = 0; index < e_interface_label_NULL; ++index)
				d_assert(result->labels[index] = GTK_LABEL(gtk_builder_get_object(result->interface, v_interface_labels[index])));
		}
	}
	return result;
}

