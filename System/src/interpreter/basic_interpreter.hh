#ifndef BASIC_INTERPRETER_HH
#define BASIC_INTERPRETER_HH

#include "infra/types.hh"

class BasicInterpreter
{
	public:
		BasicInterpreter();
		~BasicInterpreter();

	public:
		void init();

	private:
		byte* _pp;

};



#endif