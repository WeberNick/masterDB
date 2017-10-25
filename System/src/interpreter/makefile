CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh
	\

OBJ = \
	fsip_interpreter.o \
	fsm_seg_interpreter.o \


all : $(OBJ)

fsip_interpreter.o : fsip_interpreter.cc fsip_interpreter.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) fsip_interpreter.cc

fsm_seg_interpreter.o : fsm_seg_interpreter.cc fsm_seg_interpreter.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) fsm_seg_interpreter.cc

clean :
	rm -f *.o a.out


