CXXFLAGS = -Wall -Weffc++ -std=c++0x

CC = g++

INCL = -I../.. -I..

HDR = \
	\

OBJ = \
	partition.o \


all : $(OBJ)

partition.o : partition.cc partition.hh ../infra/types.hh ../interpreter/fsip_interpreter.hh
	$(CC) -c $(CXXFLAGS) $(INCL) partition.cc

clean :
	rm -f *.o a.out


