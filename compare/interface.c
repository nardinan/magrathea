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
	"v_pedestal_chart_alignment",
	"v_sigma_raw_chart_alignment",
	"v_sigma_chart_alignment",
	"v_pedestal_distance_chart_alignment"
}, *v_interface_radios[] = {
	"v_ladder_all_check",
	"v_ladder_selected_check"
}, *v_interface_spins[] = {
	"v_ladder_number"
}, *v_interface_switches[] = {
	"v_trb_ladder_button_0",
	"v_trb_ladder_button_1",
	"v_trb_ladder_button_2",
	"v_trb_ladder_button_3",
	"v_trb_ladder_button_4",
	"v_trb_ladder_button_5",
	"v_trb_ladder_button_6",
	"v_trb_ladder_button_7",
	"v_trb_ladder_button_8",
	"v_trb_ladder_button_9",
	"v_trb_ladder_button_10",
	"v_trb_ladder_button_11",
	"v_trb_ladder_button_12",
	"v_trb_ladder_button_13",
	"v_trb_ladder_button_14",
	"v_trb_ladder_button_15",
	"v_trb_ladder_button_16",
	"v_trb_ladder_button_17",
	"v_trb_ladder_button_18",
	"v_trb_ladder_button_19",
	"v_trb_ladder_button_20",
	"v_trb_ladder_button_21",
	"v_trb_ladder_button_22",
	"v_trb_ladder_button_23"
}, *v_interface_chart_labels[] = {
	"pedestal",
	"sigma_raw",
	"sigma",
	"pedestal_distance",
}, *v_interface_chart_styles[] = {
	"styles/pedestal.keys",
	"styles/sigma_raw.keys",
	"styles/sigma.keys",
	"styles/pedestal_distance.keys"
};
struct s_interface *f_interface_initialize(struct s_interface *supplied, const char *builder_path) {
	struct s_interface *result = supplied;
	int index;
	if (!result)
		if (!(result = (struct s_interface *)d_malloc(sizeof(struct s_interface))))
			d_die(d_error_malloc);
	result->destroy = d_false;
	if ((result->interface = gtk_builder_new()))
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
			for (index = 0; index < e_interface_radio_NULL; ++index)
				d_assert(result->radios[index] = GTK_RADIO_BUTTON(gtk_builder_get_object(result->interface, v_interface_radios[index])));
			for (index = 0; index < e_interface_spin_NULL; ++index)
				d_assert(result->spins[index] = GTK_SPIN_BUTTON(gtk_builder_get_object(result->interface, v_interface_spins[index])));
			for (index = 0; index < e_interface_switch_NULL; ++index)
				d_assert(result->switches[index] = GTK_TOGGLE_BUTTON(gtk_builder_get_object(result->interface, v_interface_switches[index])));
		}
	return result;
}

