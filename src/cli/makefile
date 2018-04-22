CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

LINKER = \
	-lstdc++fs \
	-lpthread \
	\

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	\

OBJ = \
	parser.o \

all : $(OBJ)

parser.o : parser.cc parser.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) parser.cc $(LINKER)

clean :
	rm -f *.o a.out