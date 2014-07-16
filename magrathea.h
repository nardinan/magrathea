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
#ifndef magrathea_magrathea_h
#define magrathea_magrathea_h
#include <signal.h>
#include "console.h"
#include "commands.h"
#include "trb_device.h"
#include "trigger_device.h"
#include "lvds_device.h"
#include "telnet_device.h"
#include "package.h"
#define d_magrathea_module(st,ds,mod) if(((st)==d_true)&&(p_magrathea_init_verbose((ds),(mod))))
#define d_magrathea_loop_sleep 25000 /* microseconds */
#define d_magrathea_version "0.1"
#define d_magrathea_exit_command "quit"
extern const char *quit_greetings[];
extern void p_magrathea_sigpipe_ignore(int signal);
extern int p_magrathea_init_verbose(int descriptor, const char *subsystem);
extern int f_magrathea_init(int descriptor);
#endif
