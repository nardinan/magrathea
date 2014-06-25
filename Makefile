objects = rs232.o plx.o trb.o console.o commands.o magrathea.o
name = magrathea
cc = gcc -g
headers = -I.. -I../miranda/
libraries = -L../miranda/
cflags = -Wall -Wno-variadic-macros -Wno-pointer-arith -c $(headers)
lflags = -Wall $(libraries) -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable)

rs232.o: rs232.c rs232.h
	$(cc) $(cflags) rs232.c

plx.o: plx.c plx.h
	$(cc) $(cflags) plx.c

trb.o: trb.c trb.h rs232.h
	$(cc) $(cflags) trb.c

console.o: console.c console.h
	$(cc) $(cflags) console.c

commands.o: commands.c commands.o trb.h plx.h
	$(cc) $(cflags) commands.c

magrathea.o: magrathea.c magrathea.h console.h commands.h
	$(cc) $(cflags) magrathea.c

clean:
	rm -f *.o
	rm -f $(executable)
