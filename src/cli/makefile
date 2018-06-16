CXXFLAGS = -Wall -std=c++17 -static-libstdc++ -g
CC = g++

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
READFLAG = -lreadline
else
READFLAG = -ledit
endif

LINKER = \
	-lstdc++fs \
	-lpthread \
	\

INCL = -I../.. -I..

HDR = \
	stime.hh \
	datejd.hh \
	linereaderedit.hh \
	stimestamp.hh \
	\
	../infra/types.hh \
	../infra/exception.hh \
	\
	../partition/partition_manager.hh \
	../segment/segment_manager.hh \
	../main/db_instance_manager.hh \
	\

OBJ = \
	stime.o \
	datejd.o \
	stimestamp.o \
	linereaderedit.o \
	parser.o \
	\

all : $(OBJ)

stime.o : stime.cc stime.hh
	$(CC) -c $(CXXFLAGS) $(INCL) stime.cc

datejd.o : datejd.cc datejd.hh
	$(CC) -c $(CXXFLAGS) $(INCL) datejd.cc

linereaderedit.o : linereaderedit.cc linereaderedit.hh datejd.hh val_tt.hh stime.hh stimestamp.hh
	$(CC) -c $(CXXFLAGS) $(INCL) linereaderedit.cc $(READFLAG)

stimestamp.o : stimestamp.cc stimestamp.hh datejd.hh stime.hh
	$(CC) -c $(CXXFLAGS) $(INCL) stimestamp.cc

parser.o : parser.cc parser.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) parser.cc $(LINKER)

clean :
	rm -f *.o a.out
