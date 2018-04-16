CXXFLAGS = -O3 -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

OBJ = \
	args.o \
	exception.o \
	\

all : $(OBJ)

args.o : args.cc args.hh types.hh argbase.hh
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc

exception.o : exception.cc exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) exception.cc

clean :
	rm -f *.o a.out


