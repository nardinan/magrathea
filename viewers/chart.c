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
#include "chart.h"
struct s_chart *f_chart_new(struct s_chart *supplied) {
	struct s_chart *result = supplied;
	if (!result)
		if (!(result = (struct s_chart *)d_malloc(sizeof(struct s_chart))))
			d_die(d_error_malloc);
	if (result->plane)
		gtk_widget_destroy(result->plane);
	if ((result->plane = gtk_drawing_area_new()))
		g_signal_connect(G_OBJECT(result->plane), "expose-event", G_CALLBACK(p_chart_callback), result);
	else
		d_die(d_error_malloc);
	return result;
}

void p_chart_create_bins(struct s_chart *chart, unsigned int code) {
	float step = fabs(chart->axis_x.range[1]-chart->axis_x.range[0])/(float)chart->bins[code], current_value;
	for (current_value = chart->axis_x.range[0]; current_value <= chart->axis_x.range[1]; current_value += step) {
		if (chart->head[code] < d_chart_bucket) {
			chart->values[code][chart->head[code]].x = current_value;
			chart->values[code][chart->head[code]].y = 0;
			chart->values[code][chart->head[code]].normalized.done = d_false;
			chart->head[code]++;
		} else
			d_log(d_log_level_default, "[WARNING] - d_chart_bucket too small");
	}
	chart->kind[code] = e_chart_kind_histogram;
}

void f_chart_append_signal(struct s_chart *chart, unsigned int code, float x, float y) {
	if (chart->head[code] < d_chart_bucket) {
		chart->values[code][chart->head[code]].x = x;
		chart->values[code][chart->head[code]].y = y;
		chart->values[code][chart->head[code]].normalized.done = d_false;
		chart->head[code]++;
	} else
		d_log(d_log_level_default, "[WARNING] - d_chart_bucket too small");
	chart->total[code] += y;
	chart->total_square[code] += y*y;
	chart->elements[code]++;
}

void f_chart_append_histogram(struct s_chart *chart, unsigned int code, float value) {
	float normalized;
	int bin;
	if (chart->kind[code] != e_chart_kind_histogram)
		p_chart_create_bins(chart, code);
	if ((value >= chart->axis_x.range[0]) && (value <= chart->axis_x.range[1])) {
		normalized = (value-chart->axis_x.range[0])/(chart->axis_x.range[1]-chart->axis_x.range[0]);
		bin = (normalized*(float)chart->bins[code]);
		chart->values[code][bin].y++;
	}
	chart->total[code] += value;
	chart->total_square[code] += value*value;
	chart->elements[code]++;
}

void f_chart_append_envelope(struct s_chart *chart, unsigned int code, float x, float max, float min) {
	chart->kind[code] = e_chart_kind_envelope;
	if (chart->head[code] < d_chart_bucket) {
		chart->values[code][chart->head[code]].x = x;
		chart->values[code][chart->head[code]].y = max;
		chart->values[code][chart->head[code]].w = min;
		chart->values[code][chart->head[code]].normalized.done = d_false;
		chart->head[code]++;
	} else
		d_log(d_log_level_default, "[WARNING] - d_chart_bucket too small");
}

void f_chart_flush(struct s_chart *chart) {
	int code;
	for (code = 0; code < d_chart_max_nested; code++) {
		chart->kind[code] = e_chart_kind_signal;
		chart->head[code] = 0;
		chart->total[code] = 0;
		chart->total_square[code] = 0;
		chart->elements[code] = 0;
	}
}

void f_chart_denormalize(struct s_chart *chart) {
	int code, index;
	for (code = 0; code < d_chart_max_nested; code++)
		for (index = 0; index < d_chart_bucket; index++)
			chart->values[code][index].normalized.done = d_false;
}

void f_chart_integerize(struct s_chart *chart) {
	float full_w = fabs(chart->axis_x.range[1]-chart->axis_x.range[0]), full_h = fabs(chart->axis_y.range[1]-chart->axis_y.range[0]), value;
	for (;; chart->axis_x.segments++) {
		value = (full_w/(float)chart->axis_x.segments);
		if ((value == (int)value) || ((int)value == 0))
			break;
	}
	for (;; chart->axis_y.segments++) {
		value = (full_h/(float)chart->axis_y.segments);
		if ((value == (int)value) || ((int)value == 0))
			break;
	}
}

void f_chart_redraw(struct s_chart *chart) {
	if (GTK_IS_WIDGET(chart->plane))
		gtk_widget_queue_draw(GTK_WIDGET(chart->plane));
}

void p_chart_redraw_axis_x(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height) {
	float x_axis_position = height, value_step = (full_w/(float)chart->axis_x.segments), position_step = (width/(float)chart->axis_x.segments),
	      current_value, current_position, current_label, last_label, size_label, real_position;
	char buffer[d_string_buffer_size];
	int written, purged;
	cairo_set_source_rgb(cr, chart->axis_x.color.R, chart->axis_x.color.G, chart->axis_x.color.B);
	cairo_set_line_width(cr, chart->axis_x.size);
	if (!d_same_sign(chart->axis_y.range[0], chart->axis_y.range[1]))
		x_axis_position = height-((abs(chart->axis_y.range[0])*height)/full_h);
	chart->normalized.x_axis = x_axis_position;
	cairo_move_to(cr, 0.0, x_axis_position);
	cairo_line_to(cr, width, x_axis_position);
	for (current_value = chart->axis_x.range[0], current_position = 0, last_label = 0; current_value < chart->axis_x.range[1]; current_value += value_step,
			current_position += position_step) {
		real_position = current_position;
		if (real_position != NAN) {
			if (((current_value < 0) && (chart->axis_x.show_negative)) ||
					((current_value >= 0) && (chart->axis_x.show_positive))) {
				if ((last_label-real_position == 0) || (real_position-last_label >= chart->axis_x.minimum_distance)) {
					purged = d_false;
					written = snprintf(buffer, d_string_buffer_size, "%.02f", current_value);
					while ((written) && (!purged)) {
						if ((buffer[written-1] == '0') || (buffer[written-1] == '.')) {
							if (buffer[written-1] == '.')
								purged = d_true;
							buffer[written-1] = '\0';
							written--;
						} else
							purged = d_true;
					}
					size_label = ((float)written*d_chart_font_size);
					if ((current_label = x_axis_position+chart->axis_x.segments_length-
								chart->axis_x.offset+d_chart_font_size) > height)
						current_label = x_axis_position-chart->axis_x.segments_length-
							chart->axis_x.offset-d_chart_font_size;
					cairo_move_to(cr, (real_position-(size_label/2.0)), current_label);
					cairo_show_text(cr, buffer);
					if (current_label > x_axis_position) {
						cairo_move_to(cr, real_position, x_axis_position-chart->axis_x.segments_length);
						cairo_line_to(cr, real_position, current_label-d_chart_font_size);
					} else {
						cairo_move_to(cr, real_position, current_label+d_chart_font_size);
						cairo_line_to(cr, real_position, x_axis_position+chart->axis_x.segments_length);
					}
					last_label = real_position;
				} else {
					cairo_move_to(cr, real_position, x_axis_position-chart->axis_x.segments_length);
					cairo_line_to(cr, real_position, x_axis_position+chart->axis_x.segments_length);
				}
			}
		}
	}
	cairo_stroke(cr);
}

void p_chart_redraw_axis_y(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height) {
	float y_axis_position = 0.0, value_step = (full_h/(float)chart->axis_y.segments), position_step = (height/(float)chart->axis_y.segments),
	      current_value, current_position, current_label, last_label, size_label, real_position;
	char buffer[d_string_buffer_size];
	int written, purged;
	cairo_set_source_rgb(cr, chart->axis_y.color.R, chart->axis_y.color.G, chart->axis_y.color.B);
	cairo_set_line_width(cr, chart->axis_y.size);
	if (!d_same_sign(chart->axis_x.range[0], chart->axis_x.range[1]))
		y_axis_position = (abs(chart->axis_x.range[0])*width)/full_w;
	chart->normalized.y_axis = y_axis_position;
	cairo_move_to(cr, y_axis_position, 0.0);
	cairo_line_to(cr, y_axis_position, height);
	for (current_value = chart->axis_y.range[0], current_position = height, last_label = height; current_value < chart->axis_y.range[1];
			current_value += value_step, current_position -= position_step) {
		real_position = current_position;
		if (real_position != NAN) {
			if (((current_value < 0) && (chart->axis_y.show_negative)) ||
					((current_value >= 0) && (chart->axis_y.show_positive))) {
				if (((last_label-real_position) == 0) || ((last_label-real_position) >= chart->axis_y.minimum_distance)) {
					purged = d_false;
					written = snprintf(buffer, d_string_buffer_size, "%.02f", current_value);
					while ((written) && (!purged)) {
						if ((buffer[written-1] == '0') || (buffer[written-1] == '.')) {
							if (buffer[written-1] == '.')
								purged = d_true;
							buffer[written-1] = '\0';
							written--;
						} else
							purged = d_true;
					}
					size_label = ((float)written*d_chart_font_size);
					if ((current_label = y_axis_position-chart->axis_y.segments_length-size_label) < 0)
						current_label = y_axis_position+chart->axis_y.segments_length+d_chart_font_size;
					cairo_move_to(cr, current_label, (real_position+chart->axis_y.offset));
					cairo_show_text(cr, buffer);
					if (current_label > y_axis_position) {
						cairo_move_to(cr, y_axis_position-chart->axis_y.segments_length, real_position);
						cairo_line_to(cr, current_label, real_position);
					} else {
						cairo_move_to(cr, current_label+size_label, real_position);
						cairo_line_to(cr, y_axis_position+chart->axis_y.segments_length, real_position);
					}
					last_label = real_position;
				} else {
					cairo_move_to(cr, y_axis_position-chart->axis_y.segments_length, real_position);
					cairo_line_to(cr, y_axis_position+chart->axis_y.segments_length, real_position);
				}
			}
		}
	}
	cairo_stroke(cr);
}

void p_chart_redraw_grid_x(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height) {
	float value_step = (full_w/(float)chart->axis_x.segments), position_step = (width/(float)chart->axis_x.segments), current_value, current_position,
	      last_label, real_position;
	const double dash_pattern[] = {1.0};
	if (chart->axis_x.show_grid) {
		cairo_set_source_rgb(cr, chart->axis_x.color.R*0.5, chart->axis_x.color.G*0.5, chart->axis_x.color.B*0.5);
		cairo_set_line_width(cr, chart->axis_x.size*0.5);
		cairo_set_dash(cr, dash_pattern, 1, 0);
		for (current_value = chart->axis_x.range[0], current_position = 0, last_label = 0; current_value < chart->axis_x.range[1];
				current_value += value_step, current_position += position_step) {
			real_position = current_position;
			if (real_position != NAN) {
				if (((current_value < 0) && (chart->axis_x.show_negative)) ||
						((current_value >= 0) && (chart->axis_x.show_positive)))
					if ((last_label-real_position == 0) || (real_position-last_label >= chart->axis_x.minimum_distance)) {
						cairo_move_to(cr, real_position, 0.0);
						cairo_line_to(cr, real_position, height);
						last_label = real_position;
					}
			}
		}
		cairo_stroke(cr);
	}
}

void p_chart_redraw_grid_y(cairo_t *cr, struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height) {
	float value_step = (full_h/(float)chart->axis_y.segments), position_step = (height/(float)chart->axis_y.segments), current_value, current_position,
	      last_label, real_position;
	const double dash_pattern[] = {1.0};
	if (chart->axis_y.show_grid) {
		cairo_set_source_rgb(cr, chart->axis_y.color.R*0.5, chart->axis_y.color.G*0.5, chart->axis_y.color.B*0.5);
		cairo_set_line_width(cr, chart->axis_y.size*0.5);
		cairo_set_dash(cr, dash_pattern, 1, 0);
		for (current_value = chart->axis_y.range[0], current_position = height, last_label = height; current_value < chart->axis_y.range[1];
				current_value += value_step, current_position -= position_step) {
			real_position = current_position;
			if (real_position != NAN) {
				if (((current_value < 0) && (chart->axis_y.show_negative)) ||
						((current_value >= 0) && (chart->axis_y.show_positive)))
					if ((last_label-real_position == 0) || (last_label-real_position >= chart->axis_y.minimum_distance)) {
						cairo_move_to(cr, 0.0, real_position);
						cairo_line_to(cr, width, real_position);
						last_label = real_position;
					}
			}
		}
		cairo_stroke(cr);
	}
}

void p_chart_normalize_switch(struct s_chart *chart, unsigned int code, unsigned int left, unsigned int right) {
	struct s_chart_value support;
	memcpy(&(support), &(chart->values[code][left]), sizeof(struct s_chart_value));
	memcpy(&(chart->values[code][left]), &(chart->values[code][right]), sizeof(struct s_chart_value));
	memcpy(&(chart->values[code][right]), &(support), sizeof(struct s_chart_value));
}

void p_chart_normalize_sort(struct s_chart *chart, unsigned int code) {
	int index, changed = d_true;
	while (changed) {
		changed = d_false;
		for (index = 0; index < (chart->head[code]-1); index++)
			if (chart->values[code][index].normalized.x < chart->values[code][index+1].normalized.x) {
				p_chart_normalize_switch(chart, code, index, index+1);
				changed = d_true;
			}
	}
}

void p_chart_normalize(struct s_chart *chart, float full_h, float full_w, unsigned int width, unsigned int height) {
	float this_x, this_y, this_w, normal_x, normal_y, normal_w, real_x, real_y, real_w;
	int index, code, normalized = d_false;
	for (code = 0; code < d_chart_max_nested; code++) {
		for (index = 0; index < chart->head[code]; index++)
			if (!chart->values[code][index].normalized.done) {
				this_x = chart->values[code][index].x;
				this_y = chart->values[code][index].y;
				this_w = chart->values[code][index].w;
				normal_x = this_x-chart->axis_x.range[0];
				normal_y = this_y-chart->axis_y.range[0];
				normal_w = this_w-chart->axis_y.range[0];
				real_x = (normal_x*width)/full_w;
				real_y = height-((normal_y*height)/full_h);
				real_w = height-((normal_w*height)/full_h);
				chart->values[code][index].normalized.x = real_x;
				chart->values[code][index].normalized.y = real_y;
				chart->values[code][index].normalized.w = real_w;
				chart->values[code][index].normalized.done = d_true;
				normalized = d_true;
			}
		if (normalized)
			p_chart_normalize_sort(chart, code);
	}
}

int p_chart_callback(GtkWidget *widget, GdkEvent *event, void *v_chart) {
	GtkAllocation dimension;
	struct s_chart *chart = (struct s_chart *)v_chart;
	float full_w = fabs(chart->axis_x.range[1]-chart->axis_x.range[0]), full_h = fabs(chart->axis_y.range[1]-chart->axis_y.range[0]),
	      arc_size = (2.0*G_PI), min_value[d_chart_max_nested] = {0}, max_value[d_chart_max_nested] = {0}, min_channel[d_chart_max_nested] = {0},
	      max_channel[d_chart_max_nested] = {0}, rms[d_chart_max_nested], pedestal[d_chart_max_nested], total_square, fraction;
	int index, code, first;
	char buffer[d_string_buffer_size];
	if ((chart->cairo_brush = gdk_cairo_create(chart->plane->window))) {
		gtk_widget_get_allocation(GTK_WIDGET(chart->plane), &dimension);
		if ((dimension.width != chart->last_width) || (dimension.height != chart->last_height)) {
			for (code = 0; code < d_chart_max_nested; code++)
				for (index = 0; index < chart->head[code]; index++)
					chart->values[code][index].normalized.done = d_false;
			chart->last_width = dimension.width;
			chart->last_height = dimension.height;
		}
		p_chart_normalize(chart, full_h, full_w, dimension.width, dimension.height);
		cairo_set_dash(chart->cairo_brush, NULL, 0, 0);
		for (code = 0; code < d_chart_max_nested; code++)
			if (chart->head[code]) {
				cairo_set_source_rgb(chart->cairo_brush, chart->data.color[code].R, chart->data.color[code].G, chart->data.color[code].B);
				cairo_set_line_width(chart->cairo_brush, chart->data.line_size[code]);
				for (index = 0, first = d_true; index < chart->head[code]; index++) {
					if (chart->values[code][index].normalized.done) {
						if (first) {
							min_value[code] = chart->values[code][index].y;
							max_value[code] = chart->values[code][index].y;
							min_channel[code] = chart->values[code][index].x;
							max_channel[code] = chart->values[code][index].x;
						} else if (chart->values[code][index].y > max_value[code]) {
							max_value[code] = chart->values[code][index].y;
							max_channel[code] = chart->values[code][index].x;
						} else if (chart->values[code][index].y < min_value[code]) {
							min_value[code] = chart->values[code][index].y;
							min_channel[code] = chart->values[code][index].x;
						}
						if (chart->kind[code] == e_chart_kind_histogram) {
							cairo_move_to(chart->cairo_brush, chart->values[code][index].normalized.x, chart->normalized.x_axis);
							cairo_line_to(chart->cairo_brush, chart->values[code][index].normalized.x,
									chart->values[code][index].normalized.y);
						} else if (chart->kind[code] == e_chart_kind_envelope) {
							cairo_move_to(chart->cairo_brush, chart->values[code][index].normalized.x,
									chart->values[code][index].normalized.y);
							cairo_line_to(chart->cairo_brush, chart->values[code][index].normalized.x,
									chart->values[code][index].normalized.w);
						} else if (!first)
							cairo_line_to(chart->cairo_brush, chart->values[code][index].normalized.x,
									chart->values[code][index].normalized.y);
						else
							cairo_move_to(chart->cairo_brush, chart->values[code][index].normalized.x,
									chart->values[code][index].normalized.y);
						cairo_arc(chart->cairo_brush, chart->values[code][index].normalized.x, chart->values[code][index].normalized.y,
								chart->data.dot_size[code], 0, arc_size);
						first = d_false;
					}
				}
				if (chart->elements) {
					pedestal[code] = (chart->total[code]/(float)chart->elements[code]);
					fraction = (1.0/(float)chart->elements[code]);
					total_square = (chart->total_square[code]*fraction);
					rms[code] = sqrtf(fabs(total_square-(pedestal[code]*pedestal[code])));
				}
				if (chart->show_borders) {
					cairo_move_to(chart->cairo_brush, (chart->border_x-d_chart_font_size), (chart->border_y+(code*d_chart_font_height)));
					cairo_show_text(chart->cairo_brush, "@");
				}
				cairo_stroke(chart->cairo_brush);
			}
		if (chart->show_borders) {
			cairo_set_source_rgb(chart->cairo_brush, 0.0, 0.0, 0.0);
			for (code = 0; code < d_chart_max_nested; code++)
				if (chart->head[code]) {
					cairo_move_to(chart->cairo_brush, chart->border_x+d_chart_font_size, (chart->border_y+(code*d_chart_font_height)));
					if (chart->kind[code] == e_chart_kind_envelope)
						snprintf(buffer, d_string_buffer_size, "min %.02f (%.0f)| max: %.02f (%.0f)", min_value[code],
								min_channel[code], max_value[code], max_channel[code]);
					else
						snprintf(buffer, d_string_buffer_size, "min: %.02f (%.0f)| max: %.02f (%.0f)| mean: %.02f | RMS: %.02f ",
								min_value[code], min_channel[code], max_value[code], max_channel[code], pedestal[code],
								rms[code]);
					cairo_show_text(chart->cairo_brush, buffer);
				}
		}
		p_chart_redraw_axis_x(chart->cairo_brush, chart, full_h, full_w, dimension.width, dimension.height);
		p_chart_redraw_axis_y(chart->cairo_brush, chart, full_h, full_w, dimension.width, dimension.height);
		p_chart_redraw_grid_x(chart->cairo_brush, chart, full_h, full_w, dimension.width, dimension.height);
		p_chart_redraw_grid_y(chart->cairo_brush, chart, full_h, full_w, dimension.width, dimension.height);
		cairo_destroy(chart->cairo_brush);
	}
	return d_true;
}
