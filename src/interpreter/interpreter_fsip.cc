#include "interpreter_fsip.hh"

size_t InterpreterFSIP::_pageSize = 0;
const CB* InterpreterFSIP::_cb = nullptr;

InterpreterFSIP::InterpreterFSIP() :
    _pp(nullptr),
    _header(nullptr)
{}

void InterpreterFSIP::init(const CB& aControlBlock)
{
    _pageSize = aControlBlock.pageSize();
    _cb = &aControlBlock;
}

void InterpreterFSIP::detach() noexcept
{
    _pp = nullptr;
    _header = nullptr;
}

void InterpreterFSIP::initNewFSIP(byte *aPP, const uint64_t aLSN,
                                  const uint32_t aPageIndex,
                                  const uint8_t aPID,
                                  const uint32_t aNoBlocks) noexcept 
{
    attach(aPP);
    uint32_t max = aNoBlocks / 32; // how far the page is free
    uint32_t i = 0;
    while (i < max)
    {
        *(((uint32_t *)aPP) + i) = 0; // set to 0
        ++i;
    }
    // only the first bits to 0, remaining to 1
    max = (_pageSize - sizeof(fsip_header_t)) / 4; // neues Limit
    uint32_t lMask = 0;
    if (i < max)
    {
        lMask = ~lMask;
        lMask = lMask << (aNoBlocks % 32);
        // former line was this, seems to be wrong
        // lMask = lMask >> (32-(aNoBlocks % 32));
        *(((uint32_t *)aPP) + i) = lMask;
        ++i;
    }

    lMask = 0;
    lMask = ~lMask; // lMask only 1s
    while (i < max)
    {
        *(((uint32_t *)aPP) + i) = lMask;
        ++i;
    }
    // set header
    uint8_t lUnused = 0;
    uint8_t lVersion = 1;
    basic_header_t lBTemp = {aLSN, aPageIndex, aPID, lVersion, lUnused, lUnused};
    fsip_header_t temp = {aNoBlocks, 0, aNoBlocks, lVersion, lUnused, lUnused, lUnused, lBTemp};
    *_header = temp;
    // debug(aPageIndex);
}

uint InterpreterFSIP::getNextFreePage() noexcept
{
    size_t lCondition = ((_pageSize - sizeof(fsip_header_t)) / 4) - 1;
    for (uint32_t j = (_header->_nextFreePage) / 32; j <= lCondition; ++j)
    {   // looping through FSIP with step 8
        uint32_t *lPP = ((uint32_t *)_pp) + j;
        uint32_t lPartBytes = *lPP; // cast to 8 Byte Int Pointer, add the next j 8Byte block and dereference
        lPartBytes = ~lPartBytes;
        if ((lPartBytes) != 0)
        {
            uint32_t lCalcFreePos = idx_lowest_bit_set<uint32_t>(lPartBytes); // find the first "leftmost" zero
            // idx_complement_bit<uint64_t>(lPP,lCalcFreePos); //set the bit to 1
            // std::cout<<" next free page is "<< (j*32) + lCalcFreePos <<std::endl;
            return ((j * 32) + lCalcFreePos);
            // change LSN
            break;
        }
    }
    return 0;
}

// TODO: added LSN and PID to param list, pls update header for allocated block
uint32_t InterpreterFSIP::getNewPage(byte *aPP, const uint64_t aLSN, const uint8_t aPID)
{
    if (_header->_freeBlocksCount == 0)
    {
        const std::string lErrMsg = std::string("No free pages on this FSIP");
        TRACE(lErrMsg);
        return INVALID_32;
        #pragma message ("TODO: Jonas: This was replaced with an return value instead of an exception throw. Delete the throw comment and this message as soon as Jonas double checked the code in partition base")
        //throw FSIPException(FLF, lErrMsg);
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
    // debug(_header->_basicHeader._pageIndex);
    return lPosFreeBlock + 1 + _header->_basicHeader._pageIndex;
}

void InterpreterFSIP::reservePage(const uint aPageIndex) noexcept
{
    uint lPageIndex = aPageIndex;
    lPageIndex -= _header->_basicHeader._pageIndex + 1;
    uint32_t *lPP = (uint32_t *)_pp;
    lPP += (lPageIndex / 32);
    uint32_t lBitindex = (lPageIndex % 32);
    uint32_t lMask = 1;
    lMask <<= lBitindex;
    // check if free
    uint32_t test = *lPP;
    test &= lMask;
    /* if (test != 0) {
        const std::string lErrMsg = std::string("FSIP was not able to reserve the requested page");
        TRACE(lErrMsg);
        throw FSIPException(FLF, lErrMsg) } */
    // reserve if free
    *lPP = *lPP | (lMask);
    --(_header->_freeBlocksCount);
    _header->_nextFreePage = getNextFreePage();
    // debug(aPageIndex);
}

void InterpreterFSIP::freePage(const uint aPageIndex) noexcept
{
    uint lPageIndex = aPageIndex;
    lPageIndex -= _header->_basicHeader._pageIndex + 1;
    TRACE("lPageIndex "+std::to_string(lPageIndex));
    TRACE("next free Page: "+std::to_string(_header->_nextFreePage));

    if (_header->_nextFreePage > lPageIndex)
    {
        _header->_nextFreePage = lPageIndex;
    }
    TRACE("next free Page: "+std::to_string(_header->_nextFreePage));

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

void InterpreterFSIP::debug(const uint aPageIndex)
{
    TRACE("debug");
    std::ofstream myfile;
    std::string filename = "page" + std::to_string(aPageIndex) + ".txt";
    myfile.open(filename);
    uint32_t *lPP2 = (uint32_t *)_pp;
    for (uint a = 0; a < _pageSize / 4; ++a)
    {
        myfile << std::hex << std::setw(8) << std::setfill('0') << *(lPP2 + a) << std::endl;
    }
    myfile.close();
}

uint32_t InterpreterFSIP::grow(const uint aNumberOfPages, const uint aMaxPagesPerFSIP) noexcept
{
    TRACE("Updating FSIP's with new partition size starts...");

    // how many pages fit on page
    uint freeOnThisPage = aMaxPagesPerFSIP - header()->_managedPages;

    if(freeOnThisPage == 0)
    {
        return aNumberOfPages;
    }
    //distance of first page to be freed to last one.
    int64_t ldist = ((int64_t)freeOnThisPage) - ((int64_t)aNumberOfPages);
    TRACE("ldist: " + std::to_string(ldist));
    byte* lPP = _pp;
    uint8_t lMask = 0;
    uint remainingPages; // to be set on this FSIP
    size_t start;
    if (ldist >= 0) // if Pages to grow fit on this page
    {
        remainingPages = aNumberOfPages;
    }
    else
    {
        // free rest of page by setting remainingPages to rest of bits.
        remainingPages = aMaxPagesPerFSIP - header()->_managedPages;
    }
    TRACE("remainingPages " + std::to_string(remainingPages));
    // free from _managedPages remainnigPages many
    header()->_freeBlocksCount += remainingPages; // mark how many new free pages there will be.
    // first byte aligned or not
    TRACE("managedPages " + std::to_string(header()->_managedPages));
        if(header()->_managedPages % 8 !=0)
        {
            // changed to shift right, negate result
            lMask = (~lMask) << (header()->_managedPages % 8);
            *(((uint8_t *)lPP) + (header()->_managedPages) / 8) = ~lMask;
            remainingPages -= 8 - (header()->_managedPages % 8);
            start = header()->_managedPages / 8 + 1;
            TRACE(std::to_string(lMask));
        }
        else
        {
            start = header()->_managedPages / 8;
        }
        TRACE("start "+std::to_string(start));
        // free all aligned bytes
        size_t i = 0;
        size_t max = remainingPages / 8;
        TRACE("max " + std::to_string(max));
        while( i < max)
        {
            *(((uint8_t *)lPP) + i + start) = 0;
            remainingPages -= 8;
            ++i;
        }
        
        TRACE("remainingPages " + std::to_string(remainingPages));
        // if there are some left
        if(remainingPages !=0 )
        {
            lMask = 0;
            // changed to shift left
            lMask = (~lMask) << remainingPages;
            TRACE(std::to_string(remainingPages)+" mask to this: "+std::to_string(lMask));
           *(((uint8_t *)lPP) + i + start) &= lMask;
        }
        // next free page is position up to which pages were managed till now.
       header()->_nextFreePage = header()->_managedPages;

        // switch the return value
    if (ldist >=0) 
    {
        header()->_managedPages += aNumberOfPages;
        // debug(header()->_basicHeader._pageIndex);

        return 0;
    }
    else
    {
        // free rest of page by setting remainingPages to rest of bits.
        // header()->_freeBlocksCount = 
        header()->_managedPages = aMaxPagesPerFSIP;
        // debug(header()->_basicHeader._pageIndex);
        return static_cast<uint32_t>((-1) * ldist); // contains pages to be managed by next fsip
    }
}
