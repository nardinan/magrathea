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
#ifndef magrathea_telnet_device_h
#define magrathea_telnet_device_h
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "adlink.h"
#include "magrathea.h"
#define d_telnet_device_stream_null -1
#define d_telnet_device_port 8090
#define d_telnet_device_queue 5
#define d_telnet_device_timeout 100 /* microseconds */
#define d_telnet_device_clients 15
typedef struct s_telnet_device_socket {
	int socket, initialized:1;
} s_telnet_device_socket;
typedef struct s_telnet_device_client {
	struct s_telnet_device_socket socket;
	char buffer[d_string_buffer_size];
	unsigned int data_pointer;
} s_telnet_device_client;
typedef struct s_telnet_device {
	struct s_telnet_device_socket socket;
	struct s_telnet_device_client clients[d_telnet_device_clients];
	struct sockaddr_in address;
	/* clients */
} s_telnet_device;
struct s_telnet_device v_telnet_device;
extern int f_telnet_device_initialize(unsigned char code);
extern void p_telnet_device_destroy_client(int client);
extern int f_telnet_device_destroy(unsigned char code);
extern void p_telnet_device_refresh_incoming(void);
extern void p_telnet_device_refresh_command(int client);
extern void p_telnet_device_refresh_readout(int client, struct s_console *console);
extern int f_telnet_device_refresh(unsigned char code, struct s_console *console);
#endif
