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
#include "components/chart.h"
typedef enum e_interface_charts {
	e_interface_chart_adc = 0,
	e_interface_chart_pedestal,
	e_interface_chart_sigma_raw,
	e_interface_chart_sigma,
	e_interface_chart_adc_pedestal,
	e_interface_chart_adc_pedestal_cn,
	e_interface_chart_signal,
	e_interface_chart_NULL
} e_interface_charts;
typedef enum e_interface_labels {
	e_interface_label_events = 0,
	e_interface_label_bad_events,
	e_interface_label_NULL
} e_interface_labels;
typedef struct s_interface {
	GtkBuilder *interface;
	GtkWindow *window;
	GtkAlignment *charts[e_interface_chart_NULL];
	GtkLabel *labels[e_interface_label_NULL];
	struct s_chart logic_charts[e_interface_chart_NULL];
	int destroy;
} s_interface;
extern const char *v_interface_charts[], *v_interface_labels[], *v_interface_chart_labels[], *v_interface_chart_styles[];
extern struct s_interface *f_interface_initialize(struct s_interface *supplied, const char *builder_path);
#endif
