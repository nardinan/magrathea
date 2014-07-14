objects = view.o interface.o package.o chart.o
name = magrathea_view
cc = gcc -g
cflags = -Wall `pkg-config --cflags gtk+-2.0` -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = `pkg-config --libs gtk+-2.0` -L../../miranda -lm -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

view.o: view.c view.h interface.h ../package.h
	$(cc) $(cflags) view.c

interface.o: interface.c interface.h chart.h
	$(cc) $(cflags) interface.c

chart.o: chart.c chart.h
	$(cc) $(cflags) chart.c

package.o: ../package.c ../package.h
	$(cc) $(cflags) ../package.c

clean:
	rm -f *.o
	rm -f $(executable)
