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
#define d_interface_default_page 3
#include "../components/chart.h"
typedef enum e_interface_charts {
	e_interface_chart_adc = 0,
	e_interface_chart_pedestal,
	e_interface_chart_sigma_raw,
	e_interface_chart_sigma,
	e_interface_chart_adc_pedestal,
	e_interface_chart_adc_pedestal_cn,
	e_interface_chart_adc_0,
	e_interface_chart_adc_1,
	e_interface_chart_adc_2,
	e_interface_chart_adc_3,
	e_interface_chart_adc_4,
	e_interface_chart_adc_5,
	e_interface_chart_adc_6,
	e_interface_chart_adc_7,
	e_interface_chart_adc_8,
	e_interface_chart_adc_9,
	e_interface_chart_adc_10,
	e_interface_chart_adc_11,
	e_interface_chart_adc_12,
	e_interface_chart_adc_13,
	e_interface_chart_adc_14,
	e_interface_chart_adc_15,
	e_interface_chart_adc_16,
	e_interface_chart_adc_17,
	e_interface_chart_adc_18,
	e_interface_chart_adc_19,
	e_interface_chart_adc_20,
	e_interface_chart_adc_21,
	e_interface_chart_adc_22,
	e_interface_chart_adc_23,
	e_interface_chart_signal_0,
	e_interface_chart_signal_1,
	e_interface_chart_signal_2,
	e_interface_chart_signal_3,
	e_interface_chart_signal_4,
	e_interface_chart_signal_5,
	e_interface_chart_signal_6,
	e_interface_chart_signal_7,
	e_interface_chart_signal_8,
	e_interface_chart_signal_9,
	e_interface_chart_signal_10,
	e_interface_chart_signal_11,
	e_interface_chart_signal_12,
	e_interface_chart_signal_13,
	e_interface_chart_signal_14,
	e_interface_chart_signal_15,
	e_interface_chart_signal_16,
	e_interface_chart_signal_17,
	e_interface_chart_signal_18,
	e_interface_chart_signal_19,
	e_interface_chart_signal_20,
	e_interface_chart_signal_21,
	e_interface_chart_signal_22,
	e_interface_chart_signal_23,
	e_interface_chart_occupancy_0,
	e_interface_chart_occupancy_1,
	e_interface_chart_occupancy_2,
	e_interface_chart_occupancy_3,
	e_interface_chart_occupancy_4,
	e_interface_chart_occupancy_5,
	e_interface_chart_occupancy_6,
	e_interface_chart_occupancy_7,
	e_interface_chart_occupancy_8,
	e_interface_chart_occupancy_9,
	e_interface_chart_occupancy_10,
	e_interface_chart_occupancy_11,
	e_interface_chart_occupancy_12,
	e_interface_chart_occupancy_13,
	e_interface_chart_occupancy_14,
	e_interface_chart_occupancy_15,
	e_interface_chart_occupancy_16,
	e_interface_chart_occupancy_17,
	e_interface_chart_occupancy_18,
	e_interface_chart_occupancy_19,
	e_interface_chart_occupancy_20,
	e_interface_chart_occupancy_21,
	e_interface_chart_occupancy_22,
	e_interface_chart_occupancy_23,
	e_interface_chart_clusters_distribution,
	e_interface_chart_packets_size_distribution,
	e_interface_chart_NULL
} e_interface_charts;
typedef enum e_interface_labels {
	e_interface_label_events = 0,
	e_interface_label_NULL
} e_interface_labels;
typedef enum e_interface_spins {
	e_interface_spin_ladder = 0,
	e_interface_spin_delay,
	e_interface_spin_NULL
} e_interface_spins;
typedef enum e_interface_buttons {
	e_interface_button_dump = 0,
	e_interface_button_redo,
	e_interface_button_last,
	e_interface_button_NULL
} e_interface_buttons;
typedef struct s_interface {
	GtkBuilder *interface;
	GtkWindow *window;
	GtkAlignment *charts[e_interface_chart_NULL];
	GtkLabel *labels[e_interface_label_NULL];
	GtkSpinButton *spins[e_interface_spin_NULL];
	GtkButton *buttons[e_interface_button_NULL];
	GtkNotebook *notebook, *quick_notebook;
	struct s_chart logic_charts[e_interface_chart_NULL];
	int destroy;
} s_interface;
extern const char *v_interface_charts[], *v_interface_labels[], *v_interface_spins[], *v_interface_buttons[], *v_interface_chart_labels[],
       *v_interface_chart_styles[];
extern struct s_interface *f_interface_initialize(struct s_interface *supplied, const char *builder_path);
#endif
