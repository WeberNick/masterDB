#ifndef FSIP_INTERPRETER_HH
#define FSIP_INTERPRETER_HH

#include "infra/types.hh"

class FSIPInterpreter
{
	//0: block is free, 1: block is occupied
	public:
		struct header_FSIP {
			uint32_t _freeBlocksCount;      // number of free Blocks in the managed part (numer of 0s)
			uint32_t _nextFreeBlock;      // index of the next 0 (indicating a free Block)
		};
		FSIPInterpreter();
		~FSIPInterpreter();


	public:
		void initNewFSIP(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID, uint32_t aNoBlocks);
		const int getBlock(byte* aPP); //rtn: partitionoffset if free, -1 if FSIP full
		void freeBlock(uint32_t aOffset);

	private:
		byte* _pp;
		static uint16_t _headersize //8 bytes

};



#endif