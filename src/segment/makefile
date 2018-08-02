CXXFLAGS = -O3 -Wall -std=c++17

CC = g++

INCL = -I../.. -I..
#Header needed by all
HDR = \
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	../infra/segment_t.hh \
	../infra/header_structs.hh \
	../buffer/buf_mngr.hh \
	../buffer/buf_cntrl_block.hh \
	\

#Header needed by 'segment_manager'
SM_HDR = \
	../partition/partition_manager.hh \
	../partition/partition_base.hh \
	../partition/partition_file.hh \
	segment_base.hh \
	segment_fsm.hh \
	segment_fsm_sp.hh \
	\
#Header needed by 'segment_base'
SB_HDR = \
	../partition/partition_base.hh \
	\
#Header needed by 'segment_fsm'
SFSM_HDR = \
        ../interpreter/interpreter_fsm.hh \
        segment_base.hh \
        \
#Header needed by 'segment_fsm_sp'
SSP_HDR = \
		   ../interpreter/interpreter_sp.hh \
		   segment_fsm.hh \
		   segment_base.hh \
		   \

OBJ = \
	segment_manager.o \
	segment_base.o \
    segment_fsm.o \
	segment_fsm_sp.o \


all : $(OBJ)

segment_manager.o : segment_manager.cc segment_manager.hh $(HDR) $(SM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_manager.cc

segment_base.o : segment_base.cc segment_base.hh $(HDR) $(SB_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_base.cc

segment_fsm.o : segment_fsm.cc segment_fsm.hh $(HDR) $(SB_HDR) $(SFSM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_fsm.cc

segment_fsm_sp.o : segment_fsm_sp.cc segment_fsm_sp.hh $(HDR) $(SB_HDR) $(SFSM_HDR) $(SSP_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_fsm_sp.cc

clean :
	rm -f *.o a.out
