CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\

OBJ = \
	partition_manager.o \
	file_partition.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh file_partition.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc

file_partition.o : file_partition.cc file_partition.hh ../interpreter/fsip_interpreter.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) file_partition.cc

clean :
	rm -f *.o a.out
