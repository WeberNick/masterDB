CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	config.hh \
	../partition/partition_manager.hh \
	../partition/file_partition.hh \
	\


OBJ = \
	../interpreter/fsip_interpreter.o \
	../partition/partition_manager.o \
	../partition/file_partition.o \
	../partition/segment_manager.o \
	../partition/segment.o \
	\

ALL = main 

all : $(ALL)

main: main.o $(OBJ)
	$(CC) -o main main.o $(OBJ)

main.o : main.cc $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc

clean :
	rm -f *.o a.out $(ALL)