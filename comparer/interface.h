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
#ifndef magrathea_interface_h
#define magrathea_interface_h
#include "../components/chart.h"
typedef enum e_interface_charts {
	e_interface_chart_pedestal = 0,
	e_interface_chart_sigma_raw,
	e_interface_chart_sigma,
	e_interface_chart_distance,
	e_interface_chart_NULL
} e_interface_charts;
typedef enum e_interface_radios {
	e_interface_radio_all = 0,
	e_interface_radio_selected,
	e_interface_radio_NULL
} e_interface_radios;
typedef enum e_interface_spins {
	e_interface_spin_ladder = 0,
	e_interface_spin_NULL
} e_interface_spins;
typedef struct s_interface {
	GtkBuilder *interface;
	GtkWindow *window;
	GtkAlignment *charts[e_interface_chart_NULL];
	GtkRadioButton *radios[e_interface_radio_NULL];
	GtkSpinButton *spins[e_interface_spin_NULL];
	struct s_chart logic_charts[e_interface_chart_NULL];
	int destroy;
} s_interface;
extern const char *v_interface_charts[], *v_interface_radios[], *v_interface_spins[], *v_interface_chart_labels[], *v_interface_chart_styles[];
extern struct s_interface *f_interface_initialize(struct s_interface *supplied, const char *builder_path);
#endif
