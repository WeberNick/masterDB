CXXFLAGS = -Wall -std=c++17 -static-libstdc++

CC = g++

LINKER = \
	-lstdc++fs \
	-lpthread \
	\

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/linereaderedit.hh \
	../partition/partition_manager.hh \
	../segment/segment_manager.hh \
	../main/db_instance_manager.hh \
	\

OBJ = \
	parser.o \

all : $(OBJ)

parser.o : parser.cc parser.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) parser.cc $(LINKER)

clean :
	rm -f *.o a.out
