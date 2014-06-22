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
#include "console.h"
#include "rs232.h"
int main (int argc, char *argv[]) {
	struct s_console *console;
	struct s_console_input input;
	struct s_console_command commands[] = {
		{
			e_console_level_guest,
			(struct s_console_parameter[]){
				{"-t", "temperature", d_false, d_false, d_true},
				{"-A", "apprendistato", d_false, d_false, d_true},
				{"", "", d_false, d_false, d_false}
			},
			"prova", "comando prova, qui ci metti una descrizione", NULL, d_true
		},
		{e_console_level_guest, NULL, "ancora", "comando ancora, qui ci metti una descrizione", NULL, d_true},
		{e_console_level_guest, NULL, "ancona", "comando ancona, qui ci metti una descrizione", NULL, d_true},
		{e_console_level_guest, NULL, "", "", NULL, d_false}
	};
	f_memory_init();
	f_console_init(&console, commands, STDIN_FILENO);
	strcpy(console->prefix, "\rCMD>");
	console->level = e_console_level_guest;
	while (d_true) {
		f_console_read(console, &input, STDOUT_FILENO, 0, 10);
	}
	f_memory_destroy();
	return 0;
}
