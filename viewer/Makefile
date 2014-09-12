objects = view.o interface.o stk_math.o package.o chart.o keys.o
name = magrathea_view
cc = gcc -g
cflags = -Wall `pkg-config --cflags gtk+-2.0` -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = `pkg-config --libs gtk+-2.0` -L../../miranda -lm -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

view.o: view.c view.h interface.h ../stk_math.h
	$(cc) $(cflags) view.c

interface.o: interface.c interface.h ../components/chart.h
	$(cc) $(cflags) interface.c

chart.o: ../components/chart.c ../components/chart.h ../components/keys.h
	$(cc) $(cflags) ../components/chart.c

keys.o: ../components/keys.c ../components/keys.h
	$(cc) $(cflags) ../components/keys.c

stk_math.o: ../stk_math.c ../stk_math.h
	$(cc) $(cflags) ../stk_math.c

package.o: ../package.c ../package.h ../trb_device.h
	$(cc) $(cflags) ../package.c

clean:
	rm -f *.o
	rm -f $(executable)
