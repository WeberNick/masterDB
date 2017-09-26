CXXFLAGS = -Wall -Weffc++ -std=c++0x

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../interpreter/fsip_interpreter.hh \
	\
	../partition/partition.hh \
	\


OBJ = \
	../interpreter/basic_interpreter.o \
	../interpreter/fsip_interpreter.o \
	../partition/partition.o \
	\

ALL = main 

all : $(ALL)

main: main.o $(OBJ)
	$(CC) -o main main.o $(OBJ)

main.o : main.cc $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc

clean :
	rm -f *.o a.out $(ALL)


#	../partition/partition_manager.hh \
#	../partition/partition_manager.o \