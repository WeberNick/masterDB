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
	BasicInterpreter lInterp = BasicInterpreter();
	uint32_t lResult=-1;
	uint32_t lReturnValue = _header._nextFreeBlock;
	for(uint32_t i=lReturnValue;(lInterp->getBlockSize()-(_headersize+lInterp->getHeaderSize())/8-1;i++){
		uint8_t lPartBits = (uint8_t) *aPP+((i-lReturnValue)*1);
		if((~lPartBits)!=0){
			for(uint8_t i= 7;i<=0;i--){
				uint8_t	lTemp= lPartBits;
				lTemp >> i;
				if((temp % 2 == 0){
					lResult=7-i;
					lResult+=lInterp.getPartitionOffset()
					uint8_t lMask=1;
					lPartBits |= (lMask << lResult);
					_nextFreeBlock=i;
					// increase LSN ... Nick?
					break;
				}
			}
			break; 
		}
	}
	return lResult;
}

void FSIPInterpreter::freeBlock(uint aOffset)
{
	aOffset-=_basicInterpreter.getPartitionOffset();

	if(_header.nextFreeBlock > aOffset){
		_header.nextFreeBlock=aOffset;
	}
	byte* lPP = _pp;
	lPP+=(aOffset/8);
	uint8_t lCurrByte = (uint8_t) *lPP;
	uint8_t lBitindex = 7-(aOffset % 8);
	uint8_t lMask = 1;
	lMask << lBitindex;
	lCurrByte &= lMask;
	//ändert das tatsächlich den Wert, oder ändert das nur was aufm Stack?
}