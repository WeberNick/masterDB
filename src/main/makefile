CXXFLAGS = -O3 -Wall -Weffc++ -std=c++17

CC = g++

LINKER = \
	-lstdc++fs \
	-lpthread \
	\

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	
MAIN_HDR = \
	../infra/args.hh \
	db_instance_manager.hh \
	test.hh \
	\

DBIM_HDR = \
	../partition/partition_manager.hh \
	../segment/segment_manager.hh \
	../segment/segment_fsm_sp.hh \
	../interpreter/interpreter_sp.hh \
	\


OBJ = \
	../infra/args.o \
	../infra/exception.o \
	../infra/trace.o \
	../partition/partition_manager.o \
	../partition/partition_base.o \
	../partition/partition_file.o \
	../partition/partition_raw.o \
	\
	../segment/segment_manager.o \
	../segment/segment_base.o \
    ../segment/segment_fsm.o \
	../segment/segment_fsm_sp.o \
	\
	../interpreter/interpreter_fsip.o \
	../interpreter/interpreter_fsm.o \
	../interpreter/interpreter_sp.o \
	\
	../buffer/buf_mngr.o \
	../buffer/buf_hash_table.o \
	../buffer/buf_cntrl_block.o \
	\
	db_instance_manager.o \
	\

ALL = main 

all : $(ALL)

main : db_instance_manager.o main.o
	$(CC) main.o $(OBJ) -o main $(LINKER)

main.o : main.cc $(HDR) $(MAIN_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc $(LINKER)

db_instance_manager.o : db_instance_manager.cc db_instance_manager.hh $(HDR) $(DBIM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) db_instance_manager.cc

clean :
	rm -f *.o a.out $(ALL)
