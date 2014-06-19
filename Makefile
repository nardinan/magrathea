objects = console.o magrathea.o
name = magrathea
cc = gcc -g
headers = -I.. -I../miranda/
libraries = -L../miranda/
cflags = -Wall -Wno-variadic-macros -Wno-pointer-arith -c $(headers)
lflags = -Wall $(libraries) -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable)

console.o: console.c console.h
	$(cc) $(cflags) console.c

magrathea.o: magrathea.c console.h
	$(cc) $(cflags) magrathea.c

clean:
	rm -f *.o
	rm -f $(executable)
