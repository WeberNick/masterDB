CXXFLAGS = -std=c++17 -mavx -Wall -Weffc++

CC = g++

INCL = -I../.. -I..

HDR = \
	measure.hh \
	\

ALL = main 

all : $(ALL)

main: main.o
	$(CC) -o main main.o

main.o : main.cc $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc

clean :
	rm -f *.o a.out $(ALL)