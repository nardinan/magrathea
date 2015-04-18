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
#include <miranda/ground.h>
#include <sys/types.h>
#include <dirent.h>
#define d_trb_device_boards 8
#define d_trb_device_ladders 24
#define d_trb_device_raw_command_size 12
#define d_trb_device_raw_command_id_byte 4
#define d_trb_device_raw_answer_size 256
#define d_trb_device_command_size 4
#define d_trb_device_sentinel_size 2
#define d_trb_device_hexadecimal_size 2
#define d_trb_device_temperatures_size 48
#define d_trb_device_timeout 5000 			/* microseconds */
#define d_trb_device_timeout_online 250000 		/* microseconds */
#define d_trb_device_timeout_refresh 10 		/* milliseconds */
#define d_trb_device_timeout_status 10 			/* seconds */
#define d_trb_device_log "magrathea.log"
#define d_trb_device_csv_character '\t'
#define B(a) v_trb_device_bytes[(a)]
#define V(a) v_trb_device_boards[(a)].status
#define d_trb_device_viewer "./viewer.sh"
#define d_trb_device_status "./status.sh"
#define d_trb_device_converter "./converter.sh"
#define d_trb_device_viewer_skip 25
#define d_trb_device_inject_prefix "ID0x"
#define d_trb_device_inject_commands 2
#define d_trb_device_inject_command_size 11
#define d_trb_device_inject_hexadecimal_size 6
#define d_trb_device_inject_timeout_command 5000	/* microseconds */
#define d_trb_device_inject_timeout_trb 100000		/* microseconds */
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
	e_trb_device_currents_null
} e_trb_device_currents;
typedef enum e_trb_device_temperatures {
	e_trb_device_temperatures_adc = 0,
	e_trb_device_temperatures_power,
	e_trb_device_temperatures_fpga_A,
	e_trb_device_temperatures_fpga_B,
	e_trb_device_temperatures_null
} e_trb_device_temperatures;
typedef enum e_trb_device_voltages {
	e_trb_device_voltages_HV1,
	e_trb_device_voltages_HV2,
	e_trb_device_voltages_null
} e_trb_device_voltages;
typedef enum e_trb_device_status {
	e_trb_device_status_mode = 0,
	e_trb_device_status_trigger_low,
	e_trb_device_status_trigger_high,
	e_trb_device_status_CN,
	e_trb_device_status_HD,
	e_trb_device_status_low_t_layer1,
	e_trb_device_status_low_t_layer2,
	e_trb_device_status_low_t_layer3,
	e_trb_device_status_low_t_layer4,
	e_trb_device_status_low_t_layer5,
	e_trb_device_status_low_t_layer6,
	e_trb_device_status_version_M,
	e_trb_device_status_version_Mlow,
	e_trb_device_status_version_L,
	e_trb_device_status_version_Llow,
	e_trb_device_status_null
} e_trb_device_status;
typedef enum e_trb_device_bytes {
	e_trb_device_bytes_board_code = 0,
	e_trb_device_bytes_command,
	e_trb_device_bytes_0x05,
	e_trb_device_bytes_0x05_current_34,
	e_trb_device_bytes_0x05_current_33,
	e_trb_device_bytes_0x05_current_57,
	e_trb_device_bytes_0x05_current_12,
	e_trb_device_bytes_0x06,
	e_trb_device_bytes_0x06_temperature_adc,
	e_trb_device_bytes_0x06_temperature_power,
	e_trb_device_bytes_0x06_temperature_fpga_A,
	e_trb_device_bytes_0x06_temperature_fpga_B,
	e_trb_device_bytes_0x06_temperature_TFH,
	e_trb_device_bytes_0x07,
	e_trb_device_bytes_0x07_current_VSSA1,
	e_trb_device_bytes_0x07_current_VSSA2,
	e_trb_device_bytes_0x07_current_VSSA3,
	e_trb_device_bytes_0x07_current_VSSA4,
	e_trb_device_bytes_0x07_current_VSSA5,
	e_trb_device_bytes_0x07_current_VSSA6,
	e_trb_device_bytes_0x07_current_33VDD1,
	e_trb_device_bytes_0x07_current_33VDD2,
	e_trb_device_bytes_0x07_control,
	e_trb_device_bytes_0x07BB_current_S34,
	e_trb_device_bytes_0x07BB_current_S33,
	e_trb_device_bytes_0x07BB_current_S57,
	e_trb_device_bytes_0x07BB_current_S12,
	e_trb_device_bytes_0x07BB_voltage_HV1,
	e_trb_device_bytes_0x07BB_voltage_HV2,
	e_trb_device_bytes_0x07AA_mode,
	e_trb_device_bytes_0x07AA_status_CN,
	e_trb_device_bytes_0x07AA_status_HD,
	e_trb_device_bytes_0x07_status_trigger_low,
	e_trb_device_bytes_0x07_status_trigger_high,
	e_trb_device_bytes_0x07_status_low_T_layer1,
	e_trb_device_bytes_0x07_status_low_T_layer2,
	e_trb_device_bytes_0x07_status_low_T_layer3,
	e_trb_device_bytes_0x07_status_low_T_layer4,
	e_trb_device_bytes_0x07_status_low_T_layer5,
	e_trb_device_bytes_0x07_status_low_T_layer6,
	e_trb_device_bytes_0x07_status_version_M,
	e_trb_device_bytes_0x07_status_version_Mlow,
	e_trb_device_bytes_0x07_status_version_L,
	e_trb_device_bytes_0x07_status_version_Llow
} e_trb_bytes;
typedef struct s_trb_status {
	float currents[e_trb_device_currents_null], temperatures[e_trb_device_temperatures_null], tfh_temperatures[d_trb_device_temperatures_size],
	      voltages[e_trb_device_voltages_null];
	unsigned char status[e_trb_device_status_null];
} s_trb_status;
typedef struct s_trb_device {
	/* do not touch */
	int descriptor, selected:1, focused:1, wrong:1;
	unsigned char code;
	const char *location;
	/* ok, now you can add your values */
	unsigned int trigger;
	struct termios old_configuration;
	time_t last_refresh, last_online;
	struct s_trb_stream {
		char destination[PATH_MAX];
		FILE *stream;
		size_t written_bytes;
	} stream;
	struct s_trb_status status;
} s_trb_device;
extern unsigned char v_trb_device_raw_head[], v_trb_device_raw_tail[];
extern unsigned int v_trb_device_bytes[];
extern const char *v_trb_device_bytes_extensions[];
extern struct s_trb_device v_trb_device_boards[d_trb_device_boards];
extern void p_trb_device_description_format(unsigned char code, char *destination, size_t size);
extern int f_trb_device_description(unsigned char code, char **tokens, size_t elements, int output);
extern void p_trb_device_status_dump(unsigned char code, const char *path);
extern int p_trb_device_status_refresh(unsigned char code);
extern int f_trb_device_status(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_stream(unsigned char code, char **tokens, size_t elements, int output);
extern void p_trb_device_write_packet(unsigned char *supplied, unsigned char code, unsigned char type, unsigned char high_byte, unsigned char low_byte);
extern int f_trb_device_write(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_mask(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_focus(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_view(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_convert(unsigned char code, char **tokens, size_t elements, int output);
extern int p_trb_device_inject_command(unsigned char code, const char *command, int output);
extern int p_trb_device_inject_file(unsigned char code, const char *file, int output);
extern int p_trb_device_inject(unsigned char code, const char *directory, int output);
extern int f_trb_device_inject(unsigned char code, char **tokens, size_t elements, int output);
extern int f_trb_device_enabled(unsigned char code);
extern int f_trb_device_initialize(unsigned char code);
extern void p_trb_device_destroy_descriptor(unsigned char code);
extern void p_trb_device_destroy_stream(unsigned char code);
extern int f_trb_device_destroy(unsigned char code);
extern void p_trb_device_refresh_analyze(unsigned char code, unsigned char *buffer, size_t size);
extern int p_trb_device_refresh_status(unsigned char code);
extern int f_trb_device_refresh(unsigned char code, struct s_console *console);
#endif
