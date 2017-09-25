#ifndef FSIP_INTERPRETER_HH
#define FSIP_INTERPRETER_HH

#include "infra/types.hh"

class FSIPInterpreter
{
	public:
		FSIPInterpreter();
		~FSIPInterpreter();

	public:
		void initNewFSIP(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID, uint32_t aNoBlocks);
		const int getBlock(byte* aPP);
		void freeBlock(uint32_t aOffset);

	private:
		byte* _pp;

};



#endif