#include "interpreter_fsip.hh"

size_t InterpreterFSIP::_pageSize = 0;
const CB* InterpreterFSIP::_cb = nullptr;

void InterpreterFSIP::init(const CB& aControlBlock) {
    _pageSize = aControlBlock.pageSize();
    _cb = &aControlBlock;
}

InterpreterFSIP::InterpreterFSIP() : _pp(nullptr), _header(nullptr) {}

void InterpreterFSIP::detach() noexcept {
    _pp = nullptr;
    _header = nullptr;
}

void InterpreterFSIP::initNewFSIP(byte *aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID,
                                  const uint32_t aNoBlocks) noexcept {
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

uint InterpreterFSIP::getNextFreePage() noexcept {
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
uint32_t InterpreterFSIP::getNewPage(byte *aPP, const uint64_t aLSN, const uint8_t aPID) {
    if (_header->_freeBlocksCount == 0) {
        const std::string lErrMsg = std::string("No free pages on this FSIP");
        TRACE(lErrMsg);
        throw FSIPException(FLF, lErrMsg);
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
    return lPosFreeBlock + 1 + _header->_basicHeader._pageIndex;
}

void InterpreterFSIP::reservePage(const uint aPageIndex) noexcept {
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
  /*  if (test != 0) {
        const std::string lErrMsg = std::string("FSIP was not able to reserve the requested page");
        TRACE(lErrMsg);
        throw FSIPException(FLF, lErrMsg);
    }*/
    // reserve if free
    *lPP = *lPP | (lMask);
    --(_header->_freeBlocksCount);
    _header->_nextFreePage = getNextFreePage();
    // debug(aPageIndex);
}

void InterpreterFSIP::freePage(const uint aPageIndex) noexcept {
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

uint32_t InterpreterFSIP::grow(const uint aNumberOfPages, const uint aMaxPagesPerFSIP) noexcept {
    //assert (aNumberOfPages >= 8)

    //get how many pages fit on page
    uint freeOnThisPage = aMaxPagesPerFSIP - header()->_managedPages;

    if(freeOnThisPage==0){
        return aNumberOfPages;
    }
    int64_t ldist = freeOnThisPage - aNumberOfPages;
    byte* lPP = _pp;
    uint8_t lMask = 0;
    uint remainingPages; //to be set on this FSIP
    size_t start;
    if (ldist >=0) //if Pages to grow fit on this page
    {
        remainingPages = aNumberOfPages;
    }
    else{
         //free rest of page by setting remainingPages to rest of bits.
        remainingPages = aMaxPagesPerFSIP - header()->_managedPages;
    }
    //free from _managedPages remainnigPages many
    header()->_freeBlocksCount += remainingPages; //mark how many new free pages there will be.
    //first byte aligned or not
        if(header()->_managedPages % 8 !=0){
            lMask = (~lMask) << (8 - (header()->_managedPages % 8));
            *(((uint8_t *)lPP) + (header()->_managedPages) / 8) = lMask;
            remainingPages -= header()->_managedPages % 8;
            start = header()->_managedPages/8 + 1;
        }
        else{
            start = header()->_managedPages/8;
        }

        //free all aligned bytes
        size_t i = 0;
        size_t max = remainingPages/8;
        while( i < max){
            *(((uint8_t *)lPP) + i + start) = 0;
            remainingPages -= 8;
            ++i;
        }

        //if there are some left
        if(remainingPages !=0 ){
            lMask = 0;
            lMask = (~lMask) >> remainingPages;
           *(((uint8_t *)lPP) + i + start) &= lMask;
        }
        //next free page is position up to which pages were managed till now.
        header()->_nextFreePage = header()->_managedPages+1;

        //switch the return value
    if (ldist >=0) 
    {
        header()->_managedPages += aNumberOfPages;
        debug(header()->_basicHeader._pageIndex);
        return 0;
    }
    else{
         //free rest of page by setting remainingPages to rest of bits.
        header()->_freeBlocksCount = 
        header()->_managedPages = aMaxPagesPerFSIP;
        return static_cast<uint32_t>((-1)* ldist); //contains pages to be managed by next fsip
    }
}
