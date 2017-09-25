CXXFLAGS = -Wall -Weffc++ -std=c++0x

CC = g++

INCL = -I../.. -I..

HDR = \
	\

OBJ = \
	fsip_interpreter.o \


all : $(OBJ)

fsip_interpreter.o : fsip_interpreter.cc fsip_interpreter.hh ../infra/types.hh
	$(CC) -c $(CXXFLAGS) $(INCL) fsip_interpreter.cc

clean :
	rm -f *.o a.out


