CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\


SM_HDR = \
	../infra/header_structs.hh \
	../partition/partition_file.hh \
	segment.hh \
	\

S_HDR = \
	../infra/header_structs.hh \
	../partition/partition_file.hh \
	\

OBJ = \
	segment_manager.o \
	segment.o \


all : $(OBJ)

segment_manager.o : segment_manager.cc segment_manager.hh $(HDR) $(SM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_manager.cc

segment.o : segment.cc segment.hh $(HDR) $(S_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment.cc

clean :
	rm -f *.o a.out
