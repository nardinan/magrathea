objects = rs232.o trb.o console.o commands.o magrathea.o
name = magrathea
cc = gcc -g
headers = -I.. -I../miranda/ -I/plx/
libraries = -L../miranda/ -L/plx/Library
cflags = -Wall -Wno-variadic-macros -Wno-pointer-arith -c $(headers)
lflags = -Wall $(libraries) -lmiranda_ground -lmagrathea_plx
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable)

rs232.o: rs232.c rs232.h
	$(cc) $(cflags) rs232.c

trb.o: trb.c trb.h rs232.h
	$(cc) $(cflags) trb.c

console.o: console.c console.h
	$(cc) $(cflags) console.c

commands.o: commands.c commands.o trb.h
	$(cc) $(cflags) commands.c

magrathea.o: magrathea.c magrathea.h console.h commands.h
	$(cc) $(cflags) magrathea.c

clean:
	rm -f *.o
	rm -f $(executable)
