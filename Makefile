ImageName = magrathea

TGT_TYPE  = App
EXTRA_CFLAGS += -I.. -I../miranda/
LFLAGS += -L../miranda/ -lmiranda_ground
PLX_OBJECTS = \
	$(OBJ_DIR)/rs232.o \
	$(OBJ_DIR)/plx.o \
	$(OBJ_DIR)/trb.o \
	$(OBJ_DIR)/console.o \
	$(OBJ_DIR)/commands.o \
	$(OBJ_DIR)/magrathea.o

#=============================================================================
# Include shared PLX makefile
#=============================================================================
include $(PLX_SDK_DIR)/Makefiles/PlxMake.def
