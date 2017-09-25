#ifndef BASIC_INTERPRETER_HH
#define BASIC_INTERPRETER_HH

#include "infra/types.hh"

class BasicInterpreter
{
	public:
		struct header_Block {
			uint64_t _LSN; 
			uint32_t _Offset; //offset from the start of the partition
			uint8_t _PID; //partition ID
			uint16_t _padding1;
			uint8_t _padding2;
	};
		BasicInterpreter();
		~BasicInterpreter();

	public:
		void init(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID);

	private:
		byte* _pp;
		static uint16_t _blocksize //4096 bytes
		static uint16_t _headersize //16 bytes

};



#endif