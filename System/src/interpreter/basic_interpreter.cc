#include "basic_interpreter.hh"

//blocksize is fixed to 4096 bytes, headersize to 16 bytes. If it changes, it has to be changed here
BasicInterpreter::BasicInterpreter() : _pp(NULL), _header(NULL), _blockSize(0)
{}

BasicInterpreter::~BasicInterpreter(){}

void BasicInterpreter::init(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID)
{
	header()->_LSN = aLSN;
	header()->_offset = aOffset;
	header()->_PID = aPID;
	header()->_padding1 = 0;
	header()->_padding2 = 0;
	header()->_padding3 = 0;

	// Oder so
	// aPP += _pagesize - sizeof(basic_header_t);
	// basic_header_t temp = {aLSN, aOffset, aPID, 0, 0, 0};
	// *(basic_header_t*)aPP = temp;
}
