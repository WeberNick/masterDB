#include "interpreter_fsip.hh"

bool InterpreterFSIP::_pageSizeSet = false;
uint16_t InterpreterFSIP::_pageSize = 4096;

void InterpreterFSIP::setPageSize(const uint16_t aPageSize) {
    if (!_pageSizeSet) {
        _pageSizeSet = !_pageSizeSet;
        _pageSize = aPageSize;
    } else {
        std::cerr << "ERROR: Page size can only be set once" << std::endl;
    }
}

InterpreterFSIP::InterpreterFSIP() : _pp(NULL), _header(NULL) {}

InterpreterFSIP::~InterpreterFSIP() {}

void InterpreterFSIP::detach() {
    _pp = NULL;
    _header = NULL;
}

void InterpreterFSIP::initNewFSIP(byte *aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID,
                                  const uint32_t aNoBlocks) {
    attach(aPP);
    uint32_t max = aNoBlocks / 32; // wie weit ist Seite frei?
    uint32_t i = 0;
    while (i < max) {
        *(((uint32_t *)aPP) + i) = 0; // setze 0
        ++i;
    }
    // nur noch die ersten bits 0, den Rest auf 1
    max = (_pageSize - sizeof(fsip_header_t)) / 4; // neues Limit
    uint32_t lMask = 0;
    if (i < max) {
        lMask = ~lMask;
        lMask = lMask >> (32 - (aNoBlocks % 32));
        *(((uint32_t *)aPP) + i) = lMask;
        ++i;
    }

    lMask = 0;
    lMask = ~lMask; // lMask nur noch 1er
    while (i < max) {
        *(((uint32_t *)aPP) + i) = lMask;
        ++i;
    }
    // header setzten
    uint8_t lUnused = 0;
    uint8_t lVersion = 1;
    basic_header_t lBTemp = {aLSN, aPageIndex, aPID, lVersion, lUnused, lUnused};
    fsip_header_t temp = {aNoBlocks, 0, aNoBlocks, lVersion, lUnused, lUnused, lUnused, lBTemp};
    *_header = temp;
    // debug(aPageIndex);
}

uint InterpreterFSIP::getNextFreePage() {
    //	std::cout<<"###### finding next free Page ######"<<std::endl;
    size_t lCondition = ((_pageSize - sizeof(fsip_header_t)) / 4) - 1;
    for (uint32_t j = (_header->_nextFreePage) / 32; j <= lCondition; ++j) { // looping through FSIP with step 8
        uint32_t *lPP = ((uint32_t *)_pp) + j;
        uint32_t lPartBytes = *lPP; // cast to 8 Byte Int Pointer, add the next j 8Byte block and dereference
        lPartBytes = ~lPartBytes;
        if ((lPartBytes) != 0) {
            uint32_t lCalcFreePos = idx_lowest_bit_set<uint32_t>(lPartBytes); // find the first "leftmost" zero
            // idx_complement_bit<uint64_t>(lPP,lCalcFreePos); //set the bit to 1
            //			std::cout<<" next free page is "<< (j*32) + lCalcFreePos <<std::endl;
            return ((j * 32) + lCalcFreePos);
            // change LSN
            break;
        }
    }
    return 0;
}

// added LSN and PID to param list, pls update header for allocated block
int InterpreterFSIP::getNewPage(byte *aPP, const uint64_t aLSN, const uint8_t aPID) {
    if (_header->_freeBlocksCount == 0) {
        return -1;
    }
    attach(aPP);
    uint32_t lPosFreeBlock = _header->_nextFreePage;
    byte *lPP = aPP;
    lPP += lPosFreeBlock / 8; // set pointer lPosfreeBlocks/8 bytes forward
    uint8_t lMask = 1;
    uint8_t lPartBits = *(uint8_t *)lPP;       // get 8 bit Int representation of the lPP byte pointer
    lPartBits |= (lMask << lPosFreeBlock % 8); // set complement bit at lPosFreeBlock in lPartBits
    *(uint8_t *)lPP = lPartBits;

    _header->_nextFreePage = getNextFreePage();
    --(_header->_freeBlocksCount);
    return lPosFreeBlock + _header->_basicHeader._pageIndex;
}

int InterpreterFSIP::reservePage(const uint aPageIndex) {
    uint lPageIndex = aPageIndex;
    lPageIndex -= _header->_basicHeader._pageIndex + 1;
    uint32_t *lPP = (uint32_t *)_pp;
    lPP += (lPageIndex / 32);
    uint32_t lBitindex = (lPageIndex % 32);
    uint32_t lMask = 1;
    lMask <<= lBitindex;
    // test if free
    uint32_t test = *lPP;
    test &= lMask;
    if (test != 0) {
        return -1;
    }
    // reserve if free
    *lPP = *lPP | (lMask);
    --(_header->_freeBlocksCount);
    _header->_nextFreePage = getNextFreePage();
    // debug(aPageIndex);
    return 0;
}

void InterpreterFSIP::freePage(const uint aPageIndex) {
    uint lPageIndex = aPageIndex;
    lPageIndex -= _header->_basicHeader._pageIndex + 1;

    if (_header->_nextFreePage > lPageIndex) {
        _header->_nextFreePage = lPageIndex;
    }

    // uint8_t lBitindex = 7 - (lPageIndex % 8);
    // uint8_t lMask = 1;
    // lMask << lBitindex;
    // lCurrByte &= lMask;
    uint32_t *lPP = (uint32_t *)_pp;
    lPP += (lPageIndex / 32);
    uint32_t lBitindex = (lPageIndex % 32);
    uint32_t lMask = 1;
    lMask <<= lBitindex;
    *lPP = *lPP & (~lMask);
    ++(_header->_freeBlocksCount);
    // debug(aPageIndex);
}

void InterpreterFSIP::debug(const uint aPageIndex) {
    std::ofstream myfile;
    std::string filename = "page" + std::to_string(aPageIndex) + ".txt";
    myfile.open(filename);
    uint32_t *lPP2 = (uint32_t *)_pp;
    for (uint a = 0; a < _pageSize / 4; ++a) {
        myfile << std::hex << *(lPP2 + a) << std::endl;
    }
    myfile.close();
}