CXXFLAGS = -O3 -Wall -std=c++17

CC = g++

INCL = -I../.. -I..

OBJ = \
	args.o \
	exception.o \
	trace.o \
	stime.o \
	datejd.o \
	linereaderedit.o \
	stimestamp.o \
	\

all : $(OBJ)

args.o : args.cc args.hh types.hh argbase.hh
	$(CC) -c $(CXXFLAGS) $(INCL) args.cc

exception.o : exception.cc exception.hh types.hh
	$(CC) -c $(CXXFLAGS) $(INCL) exception.cc

trace.o : trace.cc trace.hh types.hh exception.hh
	$(CC) -c $(CXXFLAGS) $(INCL) trace.cc

stime.o : stime.cc stime.hh
	$(CC) -c $(CXXFLAGS) $(INCL) stime.cc

datejd.o : datejd.cc datejd.hh
	$(CC) -c $(CXXFLAGS) $(INCL) datejd.cc

linereaderedit.o : linereaderedit.cc linereaderedit.hh datejd.hh val_tt.hh stime.hh stimestamp.hh
	$(CC) -c $(CXXFLAGS) $(INCL) linereaderedit.cc

stimestamp.o : stimestamp.cc stimestamp.hh datejd.hh stime.hh
	$(CC) -c $(CXXFLAGS) $(INCL) stimestamp.cc

clean :
	rm -f *.o a.out


