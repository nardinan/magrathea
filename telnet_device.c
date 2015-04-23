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
#include "telnet_device.h"
int f_telnet_device_initialize(unsigned char code) {
	int flag = 1;
	if (!v_telnet_device.socket.initialized) {
		memset(&(v_telnet_device.address), 0, sizeof(struct sockaddr_in));
		if ((v_telnet_device.socket.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != d_telnet_device_stream_null) {
			if (setsockopt(v_telnet_device.socket.socket, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int)) != d_telnet_device_stream_null) {
				v_telnet_device.address.sin_family = AF_INET;
				v_telnet_device.address.sin_port = htons(d_telnet_device_port);
				v_telnet_device.address.sin_addr.s_addr = htonl(INADDR_ANY);
				if (bind(v_telnet_device.socket.socket, (struct sockaddr *)&(v_telnet_device.address), sizeof(struct sockaddr_in)) == 0)
					if (listen(v_telnet_device.socket.socket, d_telnet_device_queue) == 0)
						v_telnet_device.socket.initialized = d_true;
			}
		}
	}
	return v_telnet_device.socket.initialized;
}

void p_telnet_device_destroy_client(int client) {
	if (v_telnet_device.clients[client].socket.initialized) {
		shutdown(v_telnet_device.clients[client].socket.socket, SHUT_RDWR);
		close(v_telnet_device.clients[client].socket.socket);
		v_telnet_device.clients[client].socket.initialized = d_false;
		v_telnet_device.clients[client].data_pointer = 0;
		memset(v_telnet_device.clients[client].buffer, 0, d_string_buffer_size);
	}
}

int f_telnet_device_destroy(unsigned char code) {
	int result = d_true, index;
	for (index = 0; index < d_telnet_device_clients; ++index)
		p_telnet_device_destroy_client(index);
	if (v_telnet_device.socket.initialized)
		close(v_telnet_device.socket.socket);
	return result;
}

void p_telnet_device_refresh_incoming(void) {
	fd_set sockets;
	struct timeval timeout = {0, d_telnet_device_timeout};
	int client, index, flags;
	if (v_telnet_device.socket.initialized) {
		FD_ZERO(&sockets);
		FD_SET(v_telnet_device.socket.socket, &sockets);
		if (select(v_telnet_device.socket.socket+1, &sockets, NULL, NULL, &timeout) > 0)
			if ((client = accept(v_telnet_device.socket.socket, NULL, NULL)) != d_telnet_device_stream_null)
				if ((flags = fcntl(client, F_GETFL)) != d_telnet_device_stream_null) {
					fcntl(client, F_SETFL, flags|O_NONBLOCK);
					for (index = 0; index < d_telnet_device_clients; index++) {
						if (!v_telnet_device.clients[index].socket.initialized) {
							v_telnet_device.clients[index].socket.initialized = d_true;
							v_telnet_device.clients[index].socket.socket = client;
							p_telnet_device_refresh_command(index);
							break;
						}
					}
					if (index == d_telnet_device_clients) {
						shutdown(client, SHUT_RDWR);
						close(client);
					}
				}
	}
}

void p_telnet_device_refresh_command(int client) {
	char buffer[d_string_buffer_size];
	snprintf(buffer, d_string_buffer_size, "[magrathea %s]>", d_magrathea_version);
	write(v_telnet_device.clients[client].socket.socket, buffer, f_string_strlen(buffer));
	fsync(v_telnet_device.clients[client].socket.socket);
}

void p_telnet_device_refresh_readout(int client, struct s_console *console) {
	fd_set sockets;
	struct timeval timeout = {0, d_telnet_device_timeout};
	char character;
	if (v_telnet_device.clients[client].socket.initialized) {
		FD_ZERO(&sockets);
		FD_SET(v_telnet_device.clients[client].socket.socket, &sockets);
		if (select(v_telnet_device.clients[client].socket.socket+1, &sockets, NULL, NULL, &timeout) > 0)
			while (read(v_telnet_device.clients[client].socket.socket, &character, 1) > 0) {
				if (character == '\n') {
					if (f_string_strlen(v_telnet_device.clients[client].buffer) > 0) {
						if (f_string_strcmp(v_telnet_device.clients[client].buffer, d_magrathea_exit_command) != 0) {
#ifdef d_telnet_echoing
							fprintf(stdout, "[<<] %s\n", v_telnet_device.clients[client].buffer);
							fflush(stdout);
#endif
							p_console_execute(console, v_telnet_device.clients[client].buffer,
									v_telnet_device.clients[client].socket.socket);
							p_telnet_device_refresh_command(client);
						} else {
							p_telnet_device_destroy_client(client);
							break;
						}
					}
					memset(v_telnet_device.clients[client].buffer, 0, d_string_buffer_size);
					v_telnet_device.clients[client].data_pointer = 0;
					break;
				} else if (character == '\b') {
					if (v_telnet_device.clients[client].data_pointer > 0)
						v_telnet_device.clients[client].buffer[--(v_telnet_device.clients[client].data_pointer)] = '\0';
				} else if ((character != '\r') && (v_telnet_device.clients[client].data_pointer < (d_string_buffer_size-1)))
					v_telnet_device.clients[client].buffer[v_telnet_device.clients[client].data_pointer++] = character;
			}
	}
}

int f_telnet_device_refresh(unsigned char code, struct s_console *console) {
	int result = d_true, index;
	if (!v_telnet_device.socket.initialized)
		f_telnet_device_initialize(code);
	if (v_telnet_device.socket.initialized) {
		p_telnet_device_refresh_incoming();
		for (index = 0; index < d_telnet_device_clients; ++index)
			if (v_telnet_device.clients[index].socket.initialized)
				p_telnet_device_refresh_readout(index, console);
	}
	return result;
}
