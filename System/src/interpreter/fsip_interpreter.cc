#include "fsip_interpreter.hh"
#include <fstream>
#include <iostream>
#include <iomanip>

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
	uint32_t max = aNoBlocks / 32; //wie weit ist Seite frei?
	uint32_t i = 0;
	while (i < max){
		*(((uint32_t*) aPP) + i)=0; //setze 0
		++i;
	}
	//nur noch die ersten bits 0, den Rest auf 1
	max = (_pageSize - sizeof(fsip_header_t))/4; //neues Limit
	uint32_t lMask = 0;
	if(i<max){
		lMask = ~lMask;
		lMask = lMask >> (32 - (aNoBlocks % 32));
		*(((uint32_t*) aPP )+ i)=lMask;
		++i;

	}

	lMask = 0;
	lMask = ~lMask; //lMask nur noch 1er
	while(i < max){
		*(((uint32_t*) aPP )+ i) = lMask;
		++i;
	}
	//header setzten
	basic_header_t lBTemp = {aLSN,aPageIndex,aPID,1,0,0};
	fsip_header_t temp = {lBTemp,aNoBlocks,0,aNoBlocks};
	*(fsip_header_t*)(aPP + _pageSize - sizeof(fsip_header_t)) = temp;

	/*
	//saving fsip to extra file
	std::ofstream myfile;
	std::string filename = "page"+std::to_string(aPageIndex)+".txt";
	myfile.open (filename);
	//std::stringstream stream;
	for(uint a=0;a<_pageSize;++a){
		//stream << std::hex << *(uint8_t*)(aPP+a);
		if(a%4==0){
			myfile << *(uint32_t*)(aPP+a) << std::endl;
		}
	}
	//std::string s = stream.str();
	//myfile << s << std::endl;
	std::cout<<sizeof(fsip_header_t)<<std::endl;
	myfile.close();*/
}

int FSIPInterpreter::getNewPage(byte* aPP, const uint64_t aLSN, const uint8_t aPID) //added LSN and PID to param list, pls update header for allocated block
{
	if(_header->_freeBlocksCount == 0){
		return -1;
	}
	attach(aPP);
	uint32_t lPosFreeBlock = _header->_nextFreeBlock;
		byte* lPP = aPP;
		lPP += lPosFreeBlock/8; // set pointer lPosfreeBlocks/8 bytes forward
		uint8_t lMask = 1;
		uint8_t lPartBits = *(uint8_t*) lPP; //get 8 bit Int representation of the lPP byte pointer 
		lPartBits |= (lMask << lPosFreeBlock % 8); //set complement bit at lPosFreeBlock in lPartBits

	size_t lCondition = ((_pageSize - sizeof(fsip_header_t))/8) - 1;
	for(uint32_t j = lPosFreeBlock/64; j <= lCondition; ++j){ //looping through FSIP with step 8 
		uint64_t* lPP = ((uint64_t*) aPP) + j;
		uint64_t lPartBytes = *lPP; //cast to 8 Byte Int Pointer, add the next j 8Byte block and dereference
		if((~lPartBytes) != 0){
			uint32_t lCalcFreePos = idx_highest_bit_set<uint64_t>(~lPartBytes); //find the first leftmost zero
			//idx_complement_bit<uint64_t>(lPP,lCalcFreePos); //set the bit to 1
			_header->_nextFreeBlock = (j*64) + lCalcFreePos;
			//change LSN
			break; 
		}
	}
	--(_header->_freeBlocksCount);
	return lPosFreeBlock + _header->_basicHeader._pageIndex;
}

int FSIPInterpreter::reservePage(const uint aPageIndex)
{
	//pls implement this
	return 0;
}

void FSIPInterpreter::freePage(const uint aPageIndex)
{
	uint lPageIndex = aPageIndex;
	lPageIndex -= _header->_basicHeader._pageIndex;

	if(_header->_nextFreeBlock > lPageIndex){
		_header->_nextFreeBlock = lPageIndex;
	}
	
	//uint8_t lBitindex = 7 - (lPageIndex % 8);
	//uint8_t lMask = 1;
	//lMask << lBitindex;
	//lCurrByte &= lMask;
	uint32_t* lPP = (uint32_t*) _pp;
	lPP += (aPageIndex / 32);
	uint32_t lBitindex = 32 - (aPageIndex % 32);
	uint32_t lMask = 1;
	lMask <<= lBitindex;
	*lPP=*lPP &(~lMask);
	++(_header->_freeBlocksCount);
	*(fsip_header_t*) (_pp+_pageSize-sizeof(fsip_header_t))=*_header;
	//ändert das tatsächlich den Wert, oder ändert das nur was aufm Stack?
}
