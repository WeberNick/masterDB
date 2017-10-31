CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	\


SM_HDR = \
	../infra/header_structs.hh \
	../partition/partition_base.hh \
	segment_base.hh \
	segment.hh \
	\

SB_HDR = \
	../partition/partition_base.hh \
	\

SFM_HDR = \
        ../infra/header_structs.hh \
        segment_base.hh \
        ../partition/partition_file.hh \
        ../interpreter/fsm_seg_interpreter.hh \
        \

S_HDR = \
	../infra/header_structs.hh \
	segment_base.hh \
	../partition/partition_file.hh \
	\

OBJ = \
	segment_manager.o \
        segment_fsm.o \
	segment_base.o \
	segment.o \


all : $(OBJ)

segment_manager.o : segment_manager.cc segment_manager.hh $(HDR) $(SM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_manager.cc

segment_base.o : segment_base.cc segment_base.hh $(HDR) $(SB_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_base.cc

segment_fsm.o : segment_fsm.cc segment_fsm.hh $(HDR) $(SB_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_fsm.cc

segment.o : segment.cc segment.hh $(HDR) $(S_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment.cc

clean :
	rm -f *.o a.out
