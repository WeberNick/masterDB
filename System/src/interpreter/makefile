CXXFLAGS = -O3 -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\

OBJ = \
	interpreter_fsip.o \
	interpreter_fsm.o \
	interpreter_sp.o \


all : $(OBJ)

interpreter_fsip.o : interpreter_fsip.cc interpreter_fsip.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) interpreter_fsip.cc

interpreter_fsm.o : interpreter_fsm.cc interpreter_fsm.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) interpreter_fsm.cc

interpreter_sp.o : interpreter_sp.cc interpreter_sp.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) interpreter_sp.cc

clean :
	rm -f *.o a.out


