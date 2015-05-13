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
#ifndef magrathea_adlink_h
#define magrathea_adlink_h
#define _FILE_OFFSET_BITS 64 /* remote 2GB file limitation */
#include "plx.h"
#include "trb_device.h"
#define d_adlink_bucket_size (1024*1024)/4
#define d_adlink_1mb (8*(1024*1024))
#define d_adlink_vendor_code 0x10b5
#define d_adlink_product_code_trigger 0x5406
#define d_adlink_product_code_data 0x9601
typedef enum e_adlink_trigger {
	e_adlink_trigger_disabled,
	e_adlink_trigger_external,
	e_adlink_trigger_20 = 20,
	e_adlink_trigger_50 = 50,
	e_adlink_trigger_100 = 100,
	e_adlink_trigger_200 = 200,
	e_adlink_trigger_300 = 300
} e_adlink_trigger;
typedef enum e_adlink_boards {
	e_adlink_boards_trigger,
	e_adlink_boards_data,
} e_adlink_boards;
typedef struct s_adlink {
	int data_connected:1, trigger_connected:1;
	PLX_PHYSICAL_MEM device_buffer;
	PLX_DEVICE_OBJECT data_device, trigger_device;
	PLX_NOTIFY_OBJECT notification;
	void *raw_memory_link;
} s_adlink;
extern struct s_adlink v_adlink_system;
extern int f_adlink_connect(enum e_adlink_boards board);
extern void f_adlink_destroy(enum e_adlink_boards board);
extern int f_adlink_trigger_setup(enum e_adlink_trigger trigger);
extern int f_adlink_data_read(unsigned int trb, size_t *readed, FILE *destination, time_t timeout);
#endif
