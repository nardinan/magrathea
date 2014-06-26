#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "PlxApi.h"
#define d_device_1mb (8*1024*1024)
#define d_device_alignment 4
#define d_device_word_size 4
#define d_true 1
#define d_false 0
PLX_STATUS f_device_select(unsigned short vendor, unsigned short ID, PLX_DEVICE_OBJECT *device) {
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

PLX_STATUS f_device_destroy(PLX_DEVICE_OBJECT *device, int channel, PLX_NOTIFY_OBJECT *notification, void **raw_device_buffer) {
	if (channel >= 0) {
		PlxPci_NotificationCancel(device, notification);
		PlxPci_DmaChannelClose(device, channel);
		PlxPci_CommonBufferUnmap(device, raw_device_buffer);
	}
	return PlxPci_DeviceClose(device);
}

PLX_STATUS f_device_prepare_dma(PLX_DEVICE_OBJECT *device, int channel, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification, 
		void **raw_device_buffer) {
	PLX_STATUS status = ApiSuccess;
	PLX_DMA_PROP dma_properties;
	PLX_INTERRUPT interrupt;
	if ((status = PlxPci_CommonBufferProperties(device, device_buffer)) == ApiSuccess)
		if ((status = PlxPci_CommonBufferMap(device, raw_device_buffer)) == ApiSuccess) {
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

PLX_STATUS f_device_write_register(PLX_DEVICE_OBJECT *device, int address, unsigned int value) {
	return PlxPci_PciBarSpaceWrite(device, 2 /* device specification (<1MB) */, address, &value, sizeof(value), BitSize32, d_true);
}

PLX_STATUS f_device_read_register(PLX_DEVICE_OBJECT *device, int address, unsigned int *value) {
	return PlxPci_PciBarSpaceRead(device, 2 /* device specification (<1MB) */, address, value, sizeof((*value)), BitSize32, d_true);
}

PLX_STATUS f_device_read_dma(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
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
				(*readed) += size;
		PlxPci_InterruptDisable(device, &interrupt);
	}
	return status;
}

int f_device_bytes_available(PLX_DEVICE_OBJECT *device, int address) {
	unsigned int value;
	int result = -1;
	if (f_device_read_register(device, address, &value) == ApiSuccess)
		if (value < d_device_1mb)
			result = (int)(value*d_device_word_size);
	return result;
}

PLX_STATUS f_device_read(PLX_DEVICE_OBJECT *device, size_t size, size_t *readed, PLX_PHYSICAL_MEM *device_buffer, PLX_NOTIFY_OBJECT *notification,
			int words_register_address, int dma_address, int dma_channel, time_t timeout) {
	size_t required;
	int available;
	PLX_STATUS status = ApiSuccess;
	(*readed) = 0;
	if ((available = f_device_bytes_available(device, words_register_address)) > 0) {
		required = (size>available)?available:size;
		if ((status = f_device_write_register(device, words_register_address, (required/4))) == ApiSuccess)
			status = f_device_read_dma(device, required, readed, device_buffer, notification, dma_address, dma_channel, timeout);
	}
	return status;
}

int f_trigger_setup(int speed, int external, int enabled) {
	struct {
		unsigned int default_level, type, repeat_counter, width, interval;
	} pulse_properties = {
		/* just real Zhangfei's configuration */
		.default_level = 1,
		.type = 2,
		.repeat_counter = (1<<12),
		.width = 5
	};
	PLX_DEVICE_OBJECT trigger;
	unsigned int value;
	if (f_device_select(0x10b5, 0x5406, &trigger) == ApiSuccess) {
		pulse_properties.interval = (1E7/speed)-pulse_properties.width;
		if (enabled) {
			f_device_write_register(&trigger, 40, pulse_properties.interval);
			value = ((pulse_properties.default_level&0x0001)<<31)|
				((pulse_properties.type&0x0003)<<29)|
				((pulse_properties.repeat_counter&0x1FFF)<<16)|
				((pulse_properties.width&0x7FFF));
			if (external)
				value |= 0x8000;
			f_device_write_register(&trigger, 44, value);
			f_device_write_register(&trigger, 0, 0x00);
		} else 
			f_device_write_register(&trigger, 0, 0x0F);
		f_device_destroy(&trigger, -1, NULL, NULL);
	}
	return d_true;
}

int main(int argc, char *argv[]) {
	PLX_PHYSICAL_MEM device_buffer;
	PLX_DEVICE_OBJECT data;
	PLX_NOTIFY_OBJECT notification;
	void *raw_device_buffer;
	unsigned int value, cycle = 0;
	size_t readed;
	FILE *output;
	if ((output = fopen("./output1.0", "wb"))) {
		memset(&device_buffer, 0, sizeof(PLX_PHYSICAL_MEM));
		if ((f_device_select(0x10b5, 0x9601, &data) == ApiSuccess)) {
			if (f_device_prepare_dma(&data, 0, &device_buffer, &notification, &raw_device_buffer) == ApiSuccess) {
				f_device_write_register(&data, (8*2*4), 0x3|0x55AA<<16|2000<<4);
				f_device_write_register(&data, (8*2*4), 0x2|0x55AA<<16|2000<<4);
				f_device_write_register(&data, (8*2*4), 0|0x55AA<<16|2000<<4);
				f_device_read_register(&data, (8*2*4), &value);
				f_trigger_setup(50, d_false, d_false);
				f_trigger_setup(300, d_false, d_true);
				do {
					usleep(1000);
					f_device_read(&data, (1024*1024*2), &readed, &device_buffer, &notification, 0, 4, 0, 10000);
					if (readed > 0) {
						printf("[%u] bytes\n", readed);
						fwrite(raw_device_buffer, readed, 1, output);
					}
				} while (cycle++ < 10000);
			}
			f_device_destroy(&data, 0, &notification, &raw_device_buffer);
		}
		fclose(output);
	}
	return 0;
}
