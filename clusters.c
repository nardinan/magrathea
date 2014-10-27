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
#include "clusters.h"
void f_clusters_init(struct s_event_environment *environment) {
	if (environment->values)
		d_free(environment->values);
	environment->values = NULL;
	environment->clusters = 0;
}

void p_clusters_search_append(struct s_event_environment *environment, struct s_cluster *cluster) {
	struct s_cluster *backup;
	if ((backup = d_realloc(environment->values, (sizeof(struct s_cluster)*(environment->clusters+1))))) {
		environment->values = backup;
		environment->values[environment->clusters] = *(cluster);
		environment->clusters++;
	}
}

void f_clusters_search(struct s_event_environment *environment, float *signals, float *sigma, float *common_noise, unsigned short *flags, float seed_k,
		float neighbour_k) {
	struct s_cluster cluster;
	int current_strip, local_strip, seed_strip = d_false, entry_point = -1, outro_point = -1;
	float current_common_noise, seed_threshold, neighbour_threshold;
	for (current_strip = 0; current_strip < d_package_channels; ++current_strip) {
		current_common_noise = common_noise[(current_strip/d_package_channels_on_va)];
		if ((seed_threshold = (sigma[current_strip]*seed_k)) <= 0)
			seed_threshold = 1.0;
		if ((neighbour_threshold = (sigma[current_strip]*neighbour_k)) <= 0)
			neighbour_threshold = 1.0;
		if ((signals[current_strip] >= neighbour_threshold) && (flags[current_strip] == 0)) {
			if (signals[current_strip] >= seed_threshold)
				seed_strip = d_true;
			if (entry_point < 0)
				entry_point = current_strip;
			outro_point = current_strip;
		} else {
			if ((seed_strip) && (outro_point >= entry_point)) {
				cluster.first_strip = entry_point;
				cluster.strips = (outro_point-entry_point)+1;
				for (local_strip = 0; local_strip < cluster.strips; ++local_strip)
					cluster.values[local_strip] = signals[local_strip+entry_point];
				cluster.common_noise = current_common_noise;
				p_clusters_search_append(environment, &cluster);
			}
			entry_point = outro_point = -1;
			seed_strip = d_false;
		}
	}
	if ((seed_strip) && (outro_point >= entry_point)) {
		cluster.first_strip = entry_point;
		cluster.strips = (outro_point-entry_point)+1;
		for (local_strip = 0; local_strip < cluster.strips; ++local_strip)
			cluster.values[local_strip] = signals[local_strip+entry_point];
		cluster.common_noise = current_common_noise;
		p_clusters_search_append(environment, &cluster);
	}

}
