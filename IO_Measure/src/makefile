CXXFLAGS = -O3 -Wall -Weffc++ -std=c++0x

CC = g++

INCL = -I../.. -I..

ALL = main 

all : $(ALL)

main: main.o
	$(CC) -o main main.o

main.o : main.cc measure.hh
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc

clean :
	rm -f *.o a.out $(ALL)