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
#include "plx.h"
PLX_STATUS f_plx_select(unsigned short vendor, unsigned short ID, PLX_DEVICE_OBJECT *device) {
	PLX_DEVICE_KEY current;
	PLX_STATUS status = ApiSuccess;
	unsigned short index = 0;
	while (status == ApiSuccess) {
		memset(&current, PCI_FIELD_IGNORE, sizeof(PLX_DEVICE_KEY));
		if ((status = PlxPci_DeviceFind(&current, index)) == ApiSuccess)
			if ((current.VendorId == vendor) && (current.DeviceId == ID)) {
				status = PlxPci_DeviceOpen(&current, device);
				break;
			}
		index++;
	}
	return status;
}

PLX_STATUS f_plx_destroy(PLX_DEVICE_OBJECT *device, int channel, PLX_NOTIFY_OBJECT *notification, void **raw_plx_buffer) {
	if (channel >= 0) {
		PlxPci_NotificationCancel(device, notification);
		PlxPci_DmaChannelClose(device, channel);
		PlxPci_CommonBufferUnmap(device, raw_plx_buffer);
	}
	return PlxPci_DeviceClose(device);
}

PLX_STATUS f_plx_prepare_dma(PLX_DEVICE_OBJECT *device, int channel, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
		void **raw_plx_buffer) {
		PLX_STATUS status = ApiSuccess;
		PLX_DMA_PROP dma_properties;
		PLX_INTERRUPT interrupt;
		if ((status = PlxPci_CommonBufferProperties(device, device_buffer)) == ApiSuccess)
			if ((status = PlxPci_CommonBufferMap(device, raw_plx_buffer)) == ApiSuccess) {
				memset(&dma_properties, 0, sizeof(PLX_DMA_PROP));
				dma_properties.ReadyInput = d_true;
				dma_properties.Burst = d_true;
				dma_properties.BurstInfinite = d_true;
				dma_properties.DoneInterrupt = d_true;
				/* device specification */
				dma_properties.RouteIntToPci = d_true;
				dma_properties.ConstAddrLocal = d_true;
				dma_properties.LocalBusWidth = 2; /* 0 - 8bit, 1 - 16bit, 2 - 32bit */
				/* end */
				if ((status = PlxPci_DmaChannelOpen(device, channel, &dma_properties)) == ApiSuccess) {
					memset(&interrupt, 0, sizeof(PLX_INTERRUPT));
					interrupt.PciMain = d_true;
					interrupt.DmaDone = (1<<channel);
					if ((status = PlxPci_InterruptDisable(device, &interrupt)) == ApiSuccess)
						if ((status = PlxPci_NotificationRegisterFor(device, &interrupt, notification)) == ApiSuccess)
							PlxPci_DeviceReset(device);
				}
			}
		return status;
	}

	PLX_STATUS f_plx_write_register(PLX_DEVICE_OBJECT *device, int address, unsigned int value) {
		return PlxPci_PciBarSpaceWrite(device, 2 /* device specification (<1MB) */, address, &value, sizeof(value), BitSize32, d_true);
	}

	PLX_STATUS f_plx_read_register(PLX_DEVICE_OBJECT *device, int address, unsigned int *value) {
		return PlxPci_PciBarSpaceRead(device, 2 /* device specification (<1MB) */, address, value, sizeof((*value)), BitSize32, d_true);
	}

	PLX_STATUS f_plx_read_dma(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
			int address, int channel, time_t timeout) {
		static PLX_INTERRUPT interrupt;
		static PLX_DMA_PARAMS dma_parameters;
		PLX_STATUS status = ApiSuccess;
		memset(&interrupt, 0, sizeof(PLX_INTERRUPT));
		interrupt.PciMain = d_true;
		interrupt.DmaDone = (1<<channel);
		if ((status = PlxPci_InterruptEnable(device, &interrupt)) == ApiSuccess) {
			memset(&dma_parameters, 0, sizeof(PLX_DMA_PARAMS));
			dma_parameters.PciAddr = device_buffer->PhysicalAddr;
			dma_parameters.LocalAddr = address;
			dma_parameters.ByteCount = size;
			dma_parameters.Direction = PLX_DMA_LOC_TO_PCI;
			if ((status = PlxPci_DmaTransferBlock(device, channel, &dma_parameters, 0)) == ApiSuccess)
				if ((status = PlxPci_NotificationWait(device, notification, timeout /* millisec */)) == ApiSuccess)
					(*readed) = size;
			PlxPci_InterruptDisable(device, &interrupt);
		}
		return status;
	}

	int f_plx_bytes_available(PLX_DEVICE_OBJECT *device, int address) {
		unsigned int value;
		int result = -1;
	if (f_plx_read_register(device, address, &value) == ApiSuccess)
		if (value < d_plx_1mb)
			result = (int)(value*d_plx_word_size);
	return result;
}

PLX_STATUS f_plx_read(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
			int words_register_address, int dma_address, int dma_channel, time_t timeout) {
	size_t required;
	int available;
	PLX_STATUS status = ApiSuccess;
	(*readed) = 0;
	if ((available = f_plx_bytes_available(device, words_register_address)) > 0) {
		required = d_plx_min(size, available);
		required = d_plx_min(required, device_buffer->Size);
		if ((status = f_plx_write_register(device, words_register_address, (required/d_plx_word_size))) == ApiSuccess)
			status = f_plx_read_dma(device, required, readed, device_buffer, notification, dma_address, dma_channel, timeout /* millisec */);
	}
	return status;
}

