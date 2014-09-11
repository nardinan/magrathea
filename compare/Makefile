objects = compare.o analyze.o interface.o chart.o keys.o
name = magrathea_compare
cc = gcc -g
cflags = -Wall `pkg-config --cflags gtk+-2.0` -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = `pkg-config --libs gtk+-2.0` -L../../miranda -lm -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

compare.o: compare.c compare.h analyze.h interface.h
	$(cc) $(cflags) compare.c

analyze.o: analyze.c analyze.h
	$(cc) $(cflags) analyze.c

interface.o: interface.c interface.h ../components/chart.h
	$(cc) $(cflags) interface.c

chart.o: ../components/chart.c ../components/chart.h ../components/keys.h
	$(cc) $(cflags) ../components/chart.c

keys.o: ../components/keys.c ../components/keys.h
	$(cc) $(cflags) ../components/keys.c

clean:
	rm -f *.o
	rm -f $(executable)
