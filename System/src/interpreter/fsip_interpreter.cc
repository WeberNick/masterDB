#include "fsip_interpreter.hh"

FSIPInterpreter::FSIPInterpreter() : _pp(NULL), _header(NULL)
{}

FSIPInterpreter::~FSIPInterpreter(){}

void FSIPInterpreter::detach()
{
	//todo
}

void FSIPInterpreter::initNewFSIP(const byte* aPP, const uint64_t aLSN, const uint32_t aOffset, const uint8_t aPID, const uint32_t aNoBlocks)
{
	//todo
}

const int FSIPInterpreter::getNewBlock(byte* aPP)
{
	//todo
	return 0;
}

void FSIPInterpreter::freeBlock(uint aOffset)
{
	//todo
}