objects = magrathea_remote.o
name = magrathea_remote
cc = gcc 
cflags = -Wall `pkg-config --cflags gtk+-2.0` -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = `pkg-config --libs gtk+-2.0` -L../../miranda -lm -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

magrathea_remote.o: magrathea_remote.c magrathea_remote.h
	$(cc) $(cflags) magrathea_remote.c

clean:
	rm -f *.o
	rm -f $(executable)
