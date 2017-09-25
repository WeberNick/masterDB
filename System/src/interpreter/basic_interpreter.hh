#ifndef BASIC_INTERPRETER_HH
#define BASIC_INTERPRETER_HH

#include "infra/types.hh"

class BasicInterpreter
{
	public:
		BasicInterpreter();
		~BasicInterpreter();

	public:
		void init(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID);

	private:
		byte* _pp;

};



#endif