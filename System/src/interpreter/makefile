CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\

OBJ = \
	basic_interpreter.o \
	fsip_interpreter.o \


all : $(OBJ)

basic_interpreter.o : basic_interpreter.cc basic_interpreter.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) basic_interpreter.cc

fsip_interpreter.o : fsip_interpreter.cc fsip_interpreter.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) fsip_interpreter.cc

clean :
	rm -f *.o a.out


