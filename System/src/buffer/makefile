CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
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
	.o \


all : $(OBJ)

partition_manager.o : partition_manager.cc partition_manager.hh $(HDR) $(PM_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_manager.cc $(LINKER)


clean :
	rm -f *.o a.out
