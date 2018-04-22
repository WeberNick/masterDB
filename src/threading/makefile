CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = \
	-lstdc++fs \
	-lpthread \
	\

INCL = -I../.. -I..
#Header needed by all
HDR = \
	../infra/types.hh \
	\

OBJ = \
	queue.o \
	pool.o \


all : $(OBJ)

queue.o : queue.cc queue.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) queue.cc $(LINKER)

pool.o : pool.cc pool.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) pool.cc $(LINKER)

clean :
	rm -f *.o a.out
