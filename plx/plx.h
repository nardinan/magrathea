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
#ifndef magrathea_plx_h
#define magrathea_plx_h
#include <stdio.h>
#include <sys/time.h>
#include <plxapi.h>
#define d_plx_1mb (8*(1024*1024))
#define d_plx_alignment 4
#define d_plx_word_size 4
#define d_true 1
#define d_false 0
export PLX_STATUS f_plx_select(unsigned short vendor, unsigned short ID, PLX_DEVICE_OBJECT *device);
export PLX_STATUS f_plx_destroy(PLX_DEVICE_OBJECT *device, int channel, PLX_NOTIFY_OBJECT *notification, void **raw_plx_buffer);
export PLX_STATUS f_plx_prepare_dma(PLX_DEVICE_OBJECT *device, int channel, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
		void **raw_plx_buffer);
export PLX_STATUS f_plx_write_register(PLX_DEVICE_OBJECT *device, int address, unsigned int value);
export PLX_STATUS f_plx_read_register(PLX_DEVICE_OBJECT *device, int address, unsigned int *value);
export PLX_STATUS f_plx_read_dma(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
		int address, int channel, time_t timeout);
extern int f_plx_bytes_available(PLX_DEVICE_OBJECT *device, int address);
extern PLX_STATUS f_plx_read(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
		int words_register_address, int dma_address, int dma_channel, time_t timeout);
#endif
