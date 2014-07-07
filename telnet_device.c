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
	if (!v_telnet_device.initialized) {
		memset(&(v_telnet_device.address), 0, sizeof(struct sockaddr_in));
		if ((v_telnet_device.socket.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != d_telnet_device_stream_null) {
			v_telnet_device.address.sin_family = AF_INET;
			v_telnet_device.address.sin_port = htons(d_telnet_device_port);
			v_telnet_device.address.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(v_telnet_device.socket, (struct sockaddr *)&(v_telnet_device.address), sizeof(struct sockaddr_in)) == 0)
				if (listen(v_telnet_device.socket, d_telnet_device_queue) == 0)
					v_telnet_device.socket.initialized = d_true;
		}
	}
	return v_telnet_device.socket.initialized;
}

int f_telnet_device_destroy(unsigned char code) {
	int result = d_true;
	/* remove every opened clients */
	if (v_telnet_device.socket.nitialized) {
		shutdown(v_telnet_device.socket.socket, SHUT_RDWR);
		close(v_telnet_device.socket.socket);
	}
	return result;
}

void p_telnet_device_refresh_incoming(unsigned char code) {
	fd_set sockets;
	struct timeval timeout = {0, d_telnet_device_timeout};
	int client, index;
	if (v_telnet_device.socket.initialized) {
		FD_ZERO(&sockets);
		FD_SET(v_telnet_device.socket.socket, &sockets);
		if (select(v_telnet_device.socket.socket+1, &sockets, NULL, NULL, &timeout) > 0)
			if ((client = accept(v_telnet_device.socket.socket, NULL, NULL)) != d_telnet_device_stream_null)
				for (index = 0; index < d_telnet_device_clients; index++) {
					if (!v_telnet_device.clients[index].initialized) {
						v_telnet_device.clients[index].initialized = d_true;
						v_telnet_device.clients[index].socket = client;
						break;
					}
				}
	}
}

void p_telnet_device_refresh_readout(unsigned char code) {
 /* TODO */
}

int f_telnet_device_refresh(unsigned char code, struct s_console *console) {
	int result = d_true;
	if (!v_telnet_device.socket.initialized)
		f_telnet_device_initialize(code);
	if (v_telnet_device.socket.initialized) {
		p_telnet_device_refresh_incoming(code);
		p_telnet_device_refresh_readout(code);
	}
	return result;
}
