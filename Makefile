ImageName = magrathea

TGT_TYPE  = App
EXTRA_CFLAGS += -D_FILE_OFFSET_BITS=64 -I.. -I../miranda/ -I/usr/local/include
LFLAGS += -L../miranda/ -L/usr/local/lib -lmiranda_ground
PLX_OBJECTS = \
	$(OBJ_DIR)/package.o \
	$(OBJ_DIR)/plx.o \
	$(OBJ_DIR)/adlink.o \
	$(OBJ_DIR)/device.o \
	$(OBJ_DIR)/trb_device.o \
	$(OBJ_DIR)/trigger_device.o \
	$(OBJ_DIR)/lvds_device.o \
	$(OBJ_DIR)/telnet_device.o \
	$(OBJ_DIR)/commands.o \
	$(OBJ_DIR)/magrathea.o \

#=============================================================================
# Include shared PLX makefile
#=============================================================================
include $(PLX_SDK_DIR)/Makefiles/PlxMake.def
