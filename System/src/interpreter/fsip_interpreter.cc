#include "fsip_interpreter.hh"

FSIPInterpreter::FSIPInterpreter() : _pp(NULL), _header(NULL), _pageSize(0)
{}

FSIPInterpreter::~FSIPInterpreter(){}

void FSIPInterpreter::detach()
{
	_pp = NULL;
	_header = NULL;
	_pageSize = 0;
}

void FSIPInterpreter::initNewFSIP(byte* aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID, const uint32_t aNoBlocks)
{
	attach(aPP);
	uint32_t max = aNoBlocks/32 //wie weit ist Seite frei?
	int uint32_t =0;
	while (i<max){
		*((uint32_t*) (aPP+i))=0; //setze 0
		++i;
	}
	//nur noch die ersten bits 0, den Rest auf 1
	uint32_t lMask = 0;
	lMask = ~lMask;
	lMask= lMask>> 32-(aNoBlocks%32);
	*((uint32_t*) (aPP+i))=lMask;
	max = _pageSize-sizeof(fsip_header_t); //neues Limit
	lMask=0;
	lMask= ~lMask; //lMask nur noch 1er
	while(i<max){
		*((uint32_t*) (aPP+i))=lMask;
	}
	//header setzten
	basic_header_t lBTemp = {aLSN,aPageIndex,aPID,1,0,0};
	fsip_header_t temp = {lBTemp,aNoBlocks,0,aNoBlocks};
	*((fsip_header_t*)aPP+_pageSize-sizeof(fsip_header_t))=temp;
}

const int FSIPInterpreter::getNewPage(byte* aPP, const uint64_t aLSN, const uint8_t aPID) //added LSN and PID to param list, pls update header for allocated block
{
	if(_header->_freeBlocksCount == 0){
		return -1;
	}
	attach(aPP);
	BasicInterpreter lPageInterp;
	uint32_t lResult = -1;
	uint32_t lPosFreeBlock = _header->_nextFreeBlock;
	byte* lPP = aPP;
	lPP+=lPosFreeBlock/8;
	uint8_t lMask = 1;
	uint8_t lPartBits = *(uint8_t*) lPP;
	lPartBits |= (lMask << lPosFreeBlock%8);

	size_t lCondition = ((_pageSize - sizeof(fsip_header_t))/8) - 1;
	for(uint32_t j = lPosFreeBlock/64; j <= lCondition; ++j){ //looping through FSIP with step 8 Bytes
		uint64_t lPartBytes = *(((uint64_t*) aPP)+j); //cast to 8 Byte Int Pointer, add the next j 8Byte block and dereference
		if((~lPartBytes) != 0){
			uint8_t lCalcFreePos = idx_lowest_bit_set<uint64_t>(~lPartBytes);
			_header->_nextFreeBlock = (j*64) + lResult;
			//change LSN
			break; 
		}
	}
	--(_header->_freeBlocksCount);
	return lPosFreeBlock + lPageInterp.getPageIndex();
}

void FSIPInterpreter::freePage(uint aPageIndex)
{
	BasicInterpreter lPageInterp;
	aPageIndex -= lPageInterp.getPageIndex();

	if(_header->_nextFreeBlock > aPageIndex){
		_header->_nextFreeBlock = aPageIndex;
	}
	byte* lPP = _pp;
	lPP += (aPageIndex / 8);
	uint8_t lCurrByte = (uint8_t) *lPP;
	uint8_t lBitindex = 7 - (aPageIndex % 8);
	uint8_t lMask = 1;
	lMask << lBitindex;
	lCurrByte &= lMask;
	++(_header->_freeBlocksCount);
	//ändert das tatsächlich den Wert, oder ändert das nur was aufm Stack?
	//Antwort: nur auf dem stack
}
