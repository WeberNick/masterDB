CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	\

PM_HDR = \
	partition_base.hh \
	partition_file.hh \
	partition_raw.hh \
	\

PF_HDR = \
	../interpreter/interpreter_fsip.hh \
	\

PR_HDR = \
	\

OBJ = \
	partition_manager.o \
	partition_base.o \
	partition_file.o \
	partition_raw.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh $(HDR) $(PM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc $(LINKER)

partition_base.o : partition_base.cc partition_base.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_base.cc $(LINKER)

partition_file.o : partition_file.cc partition_file.hh $(HDR) $(PF_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_file.cc $(LINKER)

partition_raw.o : partition_raw.cc partition_raw.hh $(HDR) $(PR_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_raw.cc $(LINKER)

clean :
	rm -f *.o a.out
