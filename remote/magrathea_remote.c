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
#include "magrathea_remote.h"
int f_remote_inject(const char *ip, const char *port, const char *command) {
	struct addrinfo hints = {AI_PASSIVE, AF_INET, SOCK_STREAM}, *response = NULL;
	int client_socket, result = d_false, flags;
	if (getaddrinfo(ip, port, &hints, &response) == 0) {
		if ((client_socket = socket(response->ai_family, response->ai_socktype, response->ai_protocol))) {
			if (connect(client_socket, response->ai_addr, response->ai_addrlen) == 0) {
				if ((flags = fcntl(client_socket, F_GETFL)) != -1) {
					fcntl(client_socket, F_SETFL, flags|O_NONBLOCK);
					write(client_socket, command, f_string_strlen(command));
					usleep(d_remote_hard_delay);
					write(client_socket, d_remote_exit_command, f_string_strlen(d_remote_exit_command));
					usleep(d_remote_hard_delay);
					result = d_true;
				}
				shutdown(client_socket, SHUT_RDWR);
			}
			close(client_socket);
		}
		freeaddrinfo(response);
	}
	return result;
}

int main (int argc, char *argv[]) {
	char buffer[d_string_buffer_size] = {0};
	int index;
	/* ./ localhost port {command} */
	if (argc >= 4) {
		for (index = 3; index < argc; ++index) {
			strncat(buffer, argv[index], (d_string_buffer_size-f_string_strlen(buffer)));
			strncat(buffer, d_remote_separator, (d_string_buffer_size-f_string_strlen(buffer)));
		}
		strncat(buffer, d_remote_final, (d_string_buffer_size-f_string_strlen(buffer)));
		if (!f_remote_inject(argv[1], argv[2], buffer))
			d_err(e_log_level_ever, "error, unable to communicate with %s:%s", argv[1], argv[2]);
	} else
		fprintf(stderr, "magrathea_remote usage:\n%s <IP> <port> {command ...}\n", argv[0]);
	return 0;
}
