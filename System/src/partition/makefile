CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\

PM_HDR = \
	file_partition.hh
	\

FP_HDR = \
	../interpreter/fsip_interpreter.hh
	\

SM_HDR = \
	../infra/header_structs.hh \
	file_partition.hh \
	segment.hh \
	\

S_HDR = \
	../infra/header_structs.hh \
	file_partition.hh \
	\

OBJ = \
	partition_manager.o \
	file_partition.o \
	segment_manager.o \
	segment.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh $(HDR) $(PM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc

file_partition.o : file_partition.cc file_partition.hh $(HDR) $(FP_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) file_partition.cc

segment_manager.o : segment_manager.cc segment_manager.hh $(HDR) $(SM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_manager.cc

segment.o : segment.cc segment.hh $(HDR) $(S_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment.cc

clean :
	rm -f *.o a.out
