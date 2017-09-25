CXXFLAGS = -Wall -Weffc++ -std=c++0x

CC = g++

INCL = -I../.. -I..

HDR = \
	\

OBJ = \
	partition_manager.o \
	partition.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh ../infra/types.hh partition.hh
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc

partition.o : partition.cc partition.hh ../infra/types.hh ../interpreter/fsip_interpreter.hh
	$(CC) -c $(CXXFLAGS) $(INCL) partition.cc

clean :
	rm -f *.o a.out


