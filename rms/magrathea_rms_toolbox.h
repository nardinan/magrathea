/*
 * magrathea
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef magrathea_rms_toolbox_h
#define magrathea_rms_toolbox_h
#include <linux/types.h>
#include <miranda/ground.h>
#include "../package.h"
#include "../calibrations.h"
#define d_rms_toolbox_command_size 4
#define d_rms_toolbox_command_row 66
#define d_rms_toolbox_filename_prefix "ID"
#define d_rms_toolbox_filename_postfix "_rms_"
#define d_rms_toolbox_filename_extension ".injlst"
#define d_rms_toolbox_bad_channels_postfix "_BAD.lst"
extern int f_rms_toolbox_write(unsigned short int trb, const char *inject_directory, unsigned char *mask);
extern int p_rms_toolbox_mask_channels_apply(unsigned short int trb, unsigned short int ladder, unsigned short int channel, const char *injected_directory);
extern int f_rms_toolbox_mask_channels(const char *channels_file, const char *inject_directory);
extern int p_rms_toolbox_generate(const char *calibration_directory, struct s_calibrations_environment *environment);
extern int f_rms_toolbox_generate(const char *calibration_directory, const char *inject_directory, float constant);
#endif
