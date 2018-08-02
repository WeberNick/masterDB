CXXFLAGS = -O3 -Wall -std=c++17

CC = g++

INCL = -I../.. -I..

TPL_HDR = \
	types.hh \
	exception.hh \
	trace.hh \
	tuple.hh \
	\

OBJ = \
	args.o \
	exception.o \
	trace.o \
	partition_t.o \
	segment_t.o \
	employee_t.o \
	\

all : $(OBJ)

args.o : args.cc args.hh types.hh argbase.hh
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc

exception.o : exception.cc exception.hh types.hh
	$(CC) -c $(CXXFLAGS) $(INCL) exception.cc

trace.o : trace.cc trace.hh types.hh exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) trace.cc

partition_t.o : partition_t.cc partition_t.hh $(TPL_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) partition_t.cc

segment_t.o : segment_t.cc segment_t.hh $(TPL_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) segment_t.cc

employee_t.o : employee_t.cc employee_t.hh $(TPL_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) employee_t.cc

clean :
	rm -f *.o a.out


