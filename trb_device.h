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
#ifndef magrathea_trb_device_h
#define magrathea_trb_device_h
#include "rs232.h"
#include "console.h"
#define d_trb_device_boards 8
#define d_trb_device_raw_command_size 12
#define d_trb_device_raw_answer_size 256
#define d_trb_device_command_size 4
#define d_trb_device_sentinel_size 2
#define d_trb_device_hexadecimal_size 2
#define d_trb_device_timeout 5000 /* microseconds */
#define d_trb_device_timeout_refresh 10
#define B(a) v_trb_device_bytes[(a)]
typedef enum e_trb_device_currents {
	e_trb_device_currents_34 = 0,
	e_trb_device_currents_33,
	e_trb_device_currents_57,
	e_trb_device_currents_12,
	e_trb_device_currents_VSSA1,
	e_trb_device_currents_VSSA2,
	e_trb_device_currents_VSSA3,
	e_trb_device_currents_VSSA4,
	e_trb_device_currents_VSSA5,
	e_trb_device_currents_VSSA6,
	e_trb_device_currents_33VDD1,
	e_trb_device_currents_33VDD2,
	e_trb_device_currents_S34,
	e_trb_device_currents_S33,
	e_trb_device_currents_null
} e_trb_device_currents;
typedef enum e_trb_device_temperatures {
	e_trb_device_temperatures_BUSA_1 = 0,
	e_trb_device_temperatures_BUSB_1,
	e_trb_device_temperatures_BUSA_2,
	e_trb_device_temperatures_BUSB_2,
	e_trb_device_temperatures_null
} e_trb_device_temperatures;
typedef enum e_trb_device_voltages {
	e_trb_device_voltages_HV1,
	e_trb_device_voltages_HV2,
	e_trb_device_voltages_null
} e_trb_device_voltages;
typedef enum e_trb_device_bytes {
	e_trb_device_bytes_board_code = 0,
	e_trb_device_bytes_command,
	e_trb_device_bytes_0x05,
	e_trb_device_bytes_0x05_current_34,
	e_trb_device_bytes_0x05_current_33,
	e_trb_device_bytes_0x05_current_57,
	e_trb_device_bytes_0x05_current_12,
	e_trb_device_bytes_0x06,
	e_trb_device_bytes_0x06_temperature_A1,
	e_trb_device_bytes_0x06_temperature_B1,
	e_trb_device_bytes_0x06_temperature_A2,
	e_trb_device_bytes_0x06_temperature_B2,
	e_trb_device_bytes_0x07,
	e_trb_device_bytes_0x07_current_VSSA1,
	e_trb_device_bytes_0x07_current_VSSA2,
	e_trb_device_bytes_0x07_current_VSSA3,
	e_trb_device_bytes_0x07_current_VSSA4,
	e_trb_device_bytes_0x07_current_VSSA5,
	e_trb_device_bytes_0x07_current_VSSA6,
	e_trb_device_bytes_0x07_current_33VDD1,
	e_trb_device_bytes_0x07_current_33VDD2,
	e_trb_device_bytes_0x07_current_S34,
	e_trb_device_bytes_0x07_current_S33,
	e_trb_device_bytes_0x07_voltage_HV1,
	e_trb_device_bytes_0x07_voltage_HV2
} e_trb_bytes;
typedef struct s_trb_device {
	int descriptor, selected:1;
	unsigned char code;
	const char *location;
	struct termios old_configuration;
	struct s_trb_stream {
		char destination[d_string_buffer_size];
		FILE *stream;
		size_t written_bytes;
	} stream;
	struct s_trb_status {
		float currents[e_trb_device_currents_null], temperatures[e_trb_device_temperatures_null], voltages[e_trb_device_voltages_null];
	} status;
} s_trb_device;
extern unsigned char v_trb_device_raw_head[], v_trb_device_raw_tail[];
extern unsigned int v_trb_device_bytes[];
extern const char *v_trb_device_bytes_extensions[];
extern struct s_trb_device v_trb_device_boards[d_trb_device_boards];
extern int f_trb_device_description(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_status(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_stream(unsigned char code, char **tokens, size_t elements, int output);
extern void p_trb_device_write_packet(unsigned char *supplied, unsigned char code, unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern int f_trb_device_write(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_mask(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_enabled(unsigned char code);
extern int f_trb_device_initialize(unsigned char code);
extern void p_trb_device_destroy_descriptor(unsigned char code);
extern void p_trb_device_destroy_stream(unsigned char code);
extern int f_trb_device_destroy(unsigned char code);
extern void p_trb_device_refresh_analyze(unsigned char code, unsigned char *buffer, size_t size);
extern int f_trb_device_refresh(unsigned char code);
#endif
