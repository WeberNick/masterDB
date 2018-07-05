#include "interpreter_fsm.hh"

size_t InterpreterFSM::_pageSize = 4096;

/**
 * @brief Construct a new InterpreterFSM::InterpreterFSM object
 * 
 */
InterpreterFSM::InterpreterFSM() :
    _pp(nullptr),
    _header(nullptr)
{}

void InterpreterFSM::setPageSize(const size_t aPageSize)
{
    _pageSize = aPageSize;
}

void InterpreterFSM::detach() noexcept
{
    _pp = nullptr;
    _header = nullptr;
}

void InterpreterFSM::initNewFSM(byte *aPP, const uint64_t aLSN,
                                const uint32_t aPageIndex, const uint8_t aPID,
                                const uint32_t aNoPages) noexcept
{
    // all 0, updagte header updaten
    uint max = (_pageSize - sizeof(fsm_header_t)) / 8;
    for (uint i = 0; i < max; ++i)
    {
        if(i != 0)
        {
            *(((uint64_t *)aPP) + i) = 0;
        }
    }
    // basic_header: LSN, PageIndex, PartitionId, Version, unused, unused
    basic_header_t lBH = { aLSN, aPageIndex, aPID, 1, 0, 0 };
    fsm_header_t lHeader = { aNoPages, 0, lBH };  
    // # why 0 nextSegment? 0 is invalid, should be correct this way
    *((fsm_header_t *)(aPP + (max * 8))) = lHeader;
}

uint32_t InterpreterFSM::getFreePage(const PageStatus aPageStatus) noexcept
{
    // search for page with sufficient free space
    // calculate new occupation
    // change status
    uint32_t i = 0;
    while (i < _header->_noPages)
    {
        PageStatus lPageStatus = getPageStatus(i);
        // if fits on page
        bool fits = 0;
        PageStatus max = PageStatus::kPageStatusSize;
        if (toType(lPageStatus) + toType(aPageStatus) < toType(max))
        {
            lPageStatus = static_cast<PageStatus>(toType(lPageStatus) + toType(aPageStatus));
            // TRACE("fits, new PageStatus is "+std::to_string(static_cast<uint>(lPageStatus)));
            changePageStatus(i, lPageStatus);
            return i;
        }
        ++i;
    }
    if (i < (_pageSize - sizeof(fsm_header_t)) * 2)
    { // add new data page to segment
        changePageStatus(i, aPageStatus);
        _header->_noPages++;
        return i;
    }
    else
    {
        return INVALID_32; // no free space on this fsm, load or create next
    }
}

void InterpreterFSM::changePageStatus(const uint aPageNo, const PageStatus aStatus) noexcept
{
    uint8_t *currByte = ((uint8_t *)_pp + aPageNo / 2);
    uint8_t lStatus = static_cast<uint8_t>(aStatus);
    if (aPageNo % 2 == 0)
    {
        *currByte &= ~15;
        *currByte |= lStatus;
    } else
    {
        lStatus <<= 4;
        *currByte &= 15;
        *currByte |= lStatus;
    }
}

PageStatus InterpreterFSM::getPageStatus(const uint aPageNo) noexcept
{
    uint8_t currByte = *((uint8_t *)_pp + aPageNo / 2);
    if (aPageNo % 2 == 0)
    {
        currByte &= 15;
    } 
    else
    {
        currByte >>= 4;
    }
    return static_cast<PageStatus>(currByte);
}


PageStatus InterpreterFSM::calcPageStatus(const uint aSizeWithoutOverhead, const uint aNoBytes) noexcept
{
    if (aSizeWithoutOverhead < aNoBytes)
    {
        return PageStatus::kNoType;
    }
  
    const uint lBucketSize = aSizeWithoutOverhead / toType(PageStatus::kMAX);
    const uint lBucketNo = std::ceil(aNoBytes / (double)lBucketSize);
    return (lBucketNo <= (uint)PageStatus::kMAX) ? static_cast<PageStatus>(lBucketNo) : PageStatus::kNoType;
}
