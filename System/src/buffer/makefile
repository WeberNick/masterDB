CXXFLAGS = -Wall -std=c++17

CC = g++

LINKER = -lstdc++fs

INCL = -I../.. -I..

HDR = \
	../infra/types.hh \
	../infra/semaphore.hh \
	\

OBJ = \
	buf_mngr.o \
	buf_hash_table.o \
	buf_cntrl_block.o \
	buf_acc_cntrl_block.o \
	\


all : $(OBJ)

buf_mngr.o : buf_mngr.cc buf_mngr.hh $(HDR) buf_hash_table.hh buf_cntrl_block.hh buf_acc_cntrl_block.hh
	$(CC) -c $(CXXFLAGS) $(INCL) buf_mngr.cc $(LINKER)

buf_hash_table.o : buf_hash_table.cc buf_hash_table.hh $(HDR) buf_cntrl_block.hh
	$(CC) -c $(CXXFLAGS) $(INCL) buf_hash_table.cc $(LINKER)

buf_cntrl_block.o : buf_cntrl_block.cc buf_cntrl_block.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) buf_cntrl_block.cc $(LINKER)

buf_acc_cntrl_block.o : buf_acc_cntrl_block.cc buf_cntrl_block.hh $(HDR)
	$(CC) -c $(CXXFLAGS) $(INCL) buf_acc_cntrl_block.cc $(LINKER)


clean :
	rm -f *.o a.out
