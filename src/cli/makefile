CXXFLAGS = -O3 -Wall -std=c++17
CC = g++

INCL = -I../.. -I..

HDR = \
	stime.hh \
	datejd.hh \
	linereaderedit.hh \
	stimestamp.hh \
	\
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	../infra/partition_t.hh \
	../infra/segment_t.hh \
	../infra/employee_t.hh \
	\
	../partition/partition_manager.hh \
	../segment/segment_manager.hh \
	../main/db_instance_manager.hh \
	../threading/pool.hh \
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
	$(CC) -c $(CXXFLAGS) $(INCL) linereaderedit.cc

stimestamp.o : stimestamp.cc stimestamp.hh datejd.hh stime.hh
	$(CC) -c $(CXXFLAGS) $(INCL) stimestamp.cc

parser.o : parser.cc parser.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) parser.cc

clean :
	rm -f *.o a.out
