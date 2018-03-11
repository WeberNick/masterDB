CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/semaphore.hh \
	\

OBJ = \
	buf_mngr.o \
	\


all : $(OBJ)

buf_mngr.o : buf_mngr.cc buf_mngr.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) buf_mngr.cc $(LINKER)

clean :
	rm -f *.o a.out
