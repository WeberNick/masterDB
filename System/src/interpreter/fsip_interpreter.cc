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
	aOffset-=_basicInterpreter.getPartitionOffset();
	byte* lPP = _pp;
	lPP+=(aOffset/8);
	uint8_t lCurrByte = (uint8_t) *lPP;
	uint8_t lBitindex = 7-(aOffset % 8);
	uint8_t lMask = 1;
	lMask << lBitindex;
	lCurrByte = lCurrByte & lMask;
	//ändert das tatsächlich den Wert, oder ändert das nur was aufm Stack?
}