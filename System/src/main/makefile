CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	../infra/args.hh \
	test.hh \
	\

DBIM_HDR = \
	../infra/types.hh \
	../partition/partition_manager.hh \
	../segment/segment_manager.hh \
	../segment/segment_fsm_sp.hh \
	../interpreter/interpreter_sp.hh \


OBJ = \
	db_instance_manager.o \
	../infra/args.o \
	../partition/partition_manager.o \
	../partition/partition_base.o \
	../partition/partition_file.o \
	../partition/partition_raw.o \
	\
	../segment/segment_manager.o \
	../segment/segment_base.o \
    ../segment/segment_fsm.o \
	../segment/segment_fsm_sp.o \
	../segment/segment.o \
	\
	../interpreter/interpreter_fsip.o \
	../interpreter/interpreter_fsm.o \
	../interpreter/interpreter_sp.o \
	\

ALL = main 

all : $(ALL)

main: db_instance_manager.o main.o $(OBJ)
	$(CC) main.o $(OBJ) -o main $(LINKER)

main.o : main.cc $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc $(LINKER)

db_instance_manager.o : db_instance_manager.cc $(DBIM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) db_instance_manager.cc $(LINKER)

clean :
	rm -f *.o a.out $(ALL)
