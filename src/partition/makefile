CXXFLAGS = -O3 -Wall -std=c++17
CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	../infra/file_util.hh \
	\

PM_HDR = \
	../infra/partition_t.hh \
	partition_base.hh \
	partition_file.hh \
	partition_raw.hh \
	\

PB_HDR = \
	../infra/header_structs.hh \
	../interpreter/interpreter_fsip.hh \
	\

PF_HDR = \
	partition_base.hh \
	../infra/partition_t.hh \
	../interpreter/interpreter_fsip.hh \
	\

PR_HDR = \
	partition_base.hh \
	../infra/partition_t.hh \
	\

OBJ = \
	partition_manager.o \
	partition_base.o \
	partition_file.o \
	partition_raw.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh $(HDR) $(PM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc

partition_base.o : partition_base.cc partition_base.hh $(HDR) $(PB_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_base.cc

partition_file.o : partition_file.cc partition_file.hh $(HDR) $(PF_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_file.cc

partition_raw.o : partition_raw.cc partition_raw.hh $(HDR) $(PR_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_raw.cc

clean :
	rm -f *.o a.out
