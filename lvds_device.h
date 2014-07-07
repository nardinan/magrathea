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
#ifndef magrathea_lvds_device_h
#define magrathea_lvds_device_h
#include "adlink.h"
#include "console.h"
#include "trb_device.h" /* it's needed to get output descriptor */
#define d_lvds_device_timeout 5 /* milliseconds */
extern int f_lvds_device_refresh(unsigned char code, struct s_console *console);
extern int f_lvds_device_initialize(unsigned char code);
extern int f_lvds_device_destroy(unsigned char code);
#endif
