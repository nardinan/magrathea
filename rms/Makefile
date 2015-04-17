objects = package.o calibrations.o magrathea_rms_toolbox.o
name = magrathea_rms_toolbox
cc = gcc -g
cflags = -Wall `pkg-config --cflags gtk+-2.0` -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = `pkg-config --libs gtk+-2.0` -L../../miranda -lm -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

package.o: ../package.c ../package.h ../trb_device.h
	$(cc) $(cflags) ../package.c

calibrations.o: ../calibrations.c ../calibrations.h ../package.h
	$(cc) $(cflags) ../calibrations.c

magrathea_rms_toolbox.o: magrathea_rms_toolbox.c magrathea_rms_toolbox.h ../package.h
	$(cc) $(cflags) magrathea_rms_toolbox.c

clean:
	rm -f *.o
	rm -f $(executable)
