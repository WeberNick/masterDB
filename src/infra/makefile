CXXFLAGS = -O3 -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

OBJ = \
	args.o \
	exception.o \
	trace.o \
	\

all : $(OBJ)

args.o : args.cc args.hh types.hh argbase.hh
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc

exception.o : exception.cc exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) exception.cc

trace.o : trace.cc trace.hh types.hh exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) trace.cc

clean :
	rm -f *.o a.out


