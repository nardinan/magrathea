objects = export.o package.o
name = firefly_export
cc = gcc -g
cflags = -Wall -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
lflags = -Wall
liblink = -L../../miranda -lmiranda_ground
executable = $(name).bin

all: $(objects)
	$(cc) $(lflags) $(objects) -o $(executable) $(liblink)

export.o: export.c export.h ../package.h
	$(cc) $(cflags) export.c

package.o: ../package.c ../package.h
	$(cc) $(cflags) ../package.c

clean:
	rm -f *.o
	rm -f $(executable)
