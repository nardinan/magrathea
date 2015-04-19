/*
 * firefly
 * Copyright (C) 2013 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "interface_scale.h"
const char *v_interface_scale_switches[] = {
	"v_informations",
	NULL
}, *v_interface_scale_scales[] = {
	"v_style_line_color_R",
	"v_style_line_color_G",
	"v_style_line_color_B",
	"v_style_background_color_R",
	"v_style_background_color_G",
	"v_style_background_color_B",
	NULL
}, *v_interface_scale_spins[] = {
	"v_y_top",
	"v_y_bottom",
	"v_y_segments",
	"v_x_top",
	"v_x_bottom",
	"v_x_segments",
	"v_style_line_size",
	"v_style_dot_size",
	NULL
};
struct s_interface_scale *f_interface_scale_new(struct s_interface_scale *result, GtkBuilder *scale_interface) {
	int index;
	if (!result)
		if (!(result = (struct s_interface_scale *) d_malloc(sizeof(struct s_interface_scale))))
			d_die(d_error_malloc);
	d_assert(result->interface = scale_interface);
	d_assert(result->window = GTK_WINDOW(gtk_builder_get_object(GTK_BUILDER(scale_interface), "v_scale_window")));
	for (index = 0; v_interface_scale_switches[index]; ++index)
		d_assert(result->switches[index] = GTK_TOGGLE_BUTTON(gtk_builder_get_object(scale_interface, v_interface_scale_switches[index])));
	for (index = 0; v_interface_scale_scales[index]; ++index)
		d_assert(result->scales[index] = GTK_SCALE(gtk_builder_get_object(scale_interface, v_interface_scale_scales[index])));
	for (index = 0; v_interface_scale_spins[index]; ++index)
		d_assert(result->spins[index] = GTK_SPIN_BUTTON(gtk_builder_get_object(scale_interface, v_interface_scale_spins[index])));
	d_assert(result->action = GTK_BUTTON(gtk_builder_get_object(scale_interface, "v_action")));
	d_assert(result->export_csv = GTK_BUTTON(gtk_builder_get_object(scale_interface, "v_export_CSV")));
	d_assert(result->export_png = GTK_BUTTON(gtk_builder_get_object(scale_interface, "v_export_PNG")));
	return result;
}

