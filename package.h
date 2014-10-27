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
#ifndef magrathea_package_h
#define magrathea_package_h
#include <miranda/ground.h>
#include "trb_device.h"
#define d_package_ladders 2
#define d_package_channels 384
#define d_package_channels_half 192
#define d_package_channels_on_va 64
#define d_package_vas 6
#define d_package_trbs 8
#define d_package_frame_header_size 8	 	/* total dimension of the header 	*/
#define d_package_frame_header_const_size 2 	/* number of constants in the header 	*/
#define d_package_frame_header_info_size 4	/* number of informations in the header */
#define d_package_frame_tail_size 2 		/* total dimension of the tail 		*/
#define d_package_data_header_size 6 		/* total dimension of the header 	*/
#define d_package_data_header_const_size 2	/* number of constants in the header 	*/
#define d_package_data_header_info_size 4	/* number of informations in the header */
#define d_package_data_tail_size 4		/* total dimension of the tail		*/
#define d_package_alignment_size 6
#define d_package_raw_workmode 3
#define d_package_nrm_workmode 2
#define d_package_raw_size (d_package_ladders*(2+(2*d_package_channels)))
#define d_package_nrm_ladder 0xcc
#define d_package_nrm_cluster 0xfc
#define d_package_buffer_size 10240
typedef struct s_package_trb {
	int id;
	unsigned char code;
} s_package_trb;
typedef enum e_package_kind {
	e_package_kind_raw,
	e_package_kind_cal,
	e_package_kind_dld
} e_package_kind;
typedef struct s_package_raw {
	unsigned short ladder[d_package_ladders], values[d_package_ladders][d_package_channels];
} s_package_raw;
typedef struct s_package_dld {
	unsigned short ladder[d_package_ladders], pedestal[d_package_ladders][d_package_channels], rms[d_package_ladders][d_package_channels];
} s_package_dld;
typedef struct s_package_nrm_ladder {
	unsigned short values[d_package_channels];
} s_package_nrm_ladder;
typedef struct s_package_nrm {
	unsigned short ladders;
	struct s_package_nrm_ladder ladders_data[d_trb_device_ladders];
} s_package_nrm;
typedef union u_package_data_values {
	struct s_package_raw raw;
	struct s_package_dld dld;
	struct s_package_nrm nrm;
} u_package_data_values;
typedef struct s_package_data {
	unsigned char kind, trb;
	unsigned short frame_length, trigger_counter, trigger_kind, sumcheck;
	union u_package_data_values values;
} s_package_data;
typedef struct s_package {
	unsigned char count, trb;
	unsigned short frame_length, sumcheck;
	struct s_package_data data;
	int complete:1, damaged:1, wrong_sumcheck:1;
} s_package;
extern struct s_package_trb v_package_trbs[];
extern unsigned char *p_package_analyze_nrm(struct s_package *package, unsigned char *buffer, size_t size);
extern unsigned char *p_package_analyze_raw(struct s_package *package, unsigned char *buffer, size_t size);
extern unsigned char *p_package_analyze_header_data(struct s_package *package, unsigned char *buffer, size_t size);
extern unsigned char *p_package_analyze_header(struct s_package *package, unsigned char *buffer, size_t size);
extern unsigned char *p_package_analyze(struct s_package *package, unsigned char *buffer, size_t size);
extern unsigned char *f_package_analyze(struct s_package *package, unsigned char *buffer, size_t size);
#endif
