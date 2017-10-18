CXXFLAGS = -Wall -Weffc++ -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/args.hh \
	test.hh \
	\


OBJ = \
	../infra/args.o \
	../partition/partition_manager.o \
	../partition/partition_base.o \
	../partition/partition_file.o \
	../partition/partition_raw.o \
	\
	../segment/segment_manager.o \
	../segment/segment_base.o \
	../segment/segment.o \
	\
	../interpreter/fsip_interpreter.o \
	\

ALL = main 

all : $(ALL)

main: main.o $(OBJ)
	$(CC) -o main main.o $(OBJ)

main.o : main.cc $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) main.cc

clean :
	rm -f *.o a.out $(ALL)