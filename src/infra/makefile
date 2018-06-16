CXXFLAGS = -O3 -Wall -std=c++17 -g

CC = g++

INCL = -I../.. -I..

OBJ = \
	args.o \
	exception.o \
	trace.o \
	tuples.o \
	\

all : $(OBJ)

args.o : args.cc args.hh types.hh argbase.hh
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc

exception.o : exception.cc exception.hh types.hh
	$(CC) -c $(CXXFLAGS) $(INCL) exception.cc

trace.o : trace.cc trace.hh types.hh exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) trace.cc

tuples.o : tuples.cc tuples.hh types.hh exception.hh trace.hh
	$(CC) -c $(CXXFLAGS) $(INCL) tuples.cc

clean :
	rm -f *.o a.out


