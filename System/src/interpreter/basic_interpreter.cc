#include "basic_interpreter.hh"

BasicInterpreter::BasicInterpreter() : _pp(NULL), _header(NULL), _pageSize(0)
{}

BasicInterpreter::~BasicInterpreter(){}

void BasicInterpreter::detach()
{
	_pp = NULL;
	_header = NULL;
	_pageSize = 0;
}

void BasicInterpreter::init(byte* aPP, uint64_t aLSN, uint32_t aPageIndex, uint8_t aPID)
{
	header()->_LSN = aLSN;
	header()->_pageIndex = aPageIndex;
	header()->_PID = aPID;
	header()->_version = 0;
	header()->_unused1 = 0;
	header()->_unused2 = 0;
}
