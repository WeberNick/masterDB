CXXFLAGS = -Wall -std=c++17

CC = g++

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/exception.hh \
	../infra/trace.hh \
	\

BUF_MNGR_HDR = \
	buf_hash_table.hh \
	buf_cntrl_block.hh \
	\
	../partition/partition_manager.hh \
	../partition/partition_base.hh \
	\

OBJ = \
	buf_mngr.o \
	buf_hash_table.o \
	buf_cntrl_block.o \
	\


all : $(OBJ)

buf_mngr.o : buf_mngr.cc buf_mngr.hh $(HDR) $(BUF_MNGR_HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) buf_mngr.cc

buf_hash_table.o : buf_hash_table.cc buf_hash_table.hh $(HDR) buf_cntrl_block.hh
	$(CC) -c $(CXXFLAGS) $(INCL) buf_hash_table.cc

buf_cntrl_block.o : buf_cntrl_block.cc buf_cntrl_block.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) buf_cntrl_block.cc

clean :
	rm -f *.o a.out
