objects = convert.o package.o
name = magrathea_convert
cc = gcc -g
cpp = g++ -g
cflags = -Wall -I../../ -Wno-variadic-macros -Wno-pointer-arith -c
cflags_cpp = $(cflags) `root-config --cflags`
lflags = -Wall
liblink = -L../../miranda -lmiranda_ground
liblink_cpp = $(liblink) `root-config --libs`
executable = $(name).bin

all: $(objects)
	$(cpp) $(lflags) $(objects) -o $(executable) $(liblink_cpp)

convert.o: convert.cpp convert.h ../package.h
	$(cpp) $(cflags_cpp) convert.cpp

package.o: ../package.c ../package.h
	$(cc) $(cflags) ../package.c

clean:
	rm -f *.o
	rm -f $(executable)
