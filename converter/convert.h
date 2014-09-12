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
#ifndef magrathea_convert_h
#define magrathea_convert_h
#include <TCanvas.h>
#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>
extern "C" {
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "../package.h"
#include "../trb_device.h"
}
#define d_convert_directory "ttree"
extern int tree_adc[d_package_ladders][d_package_channels];
typedef struct s_convert_environment {
	TFile *stream[d_package_ladders];
	TTree *structure[d_package_ladders];
} s_convert_environment;
extern struct s_convert_environment *f_convert_init(struct s_convert_environment *supplied, const char *prefix);
extern void f_convert_destroy(struct s_convert_environment *supplied);
extern int f_convert_insert(struct s_convert_environment *environment, struct s_package *package);
extern int f_convert_read(const char *prefix, FILE *stream, int trb);
#endif
