CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	types.hh \
	argbase.hh \
	\

OBJ = \
	args.o \

all : $(OBJ)

args.o : args.cc args.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc $(LINKER)

clean :
	rm -f *.o a.out


