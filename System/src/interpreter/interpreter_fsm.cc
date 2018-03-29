#include "interpreter_fsm.hh"


bool InterpreterFSM::_pageSizeSet = false;
uint16_t InterpreterFSM::_pageSize = 4096;

void InterpreterFSM::setPageSize(const uint16_t aPageSize) {
    if (!_pageSizeSet) {
        _pageSizeSet = !_pageSizeSet;
        _pageSize = aPageSize;
    } else {
        std::cerr << "ERROR: Page size can only be set once" << std::endl;
    }
}

InterpreterFSM::InterpreterFSM() : _pp(NULL), _header(NULL) {}

InterpreterFSM::~InterpreterFSM() {}

void InterpreterFSM::detach() {
    _pp = NULL;
    _header = NULL;
}

void InterpreterFSM::initNewFSM(byte *aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID,
                                const uint32_t aNoPages) {
    // alles 0, header updaten
    uint max = (_pageSize - sizeof(fsm_header_t)) / 8;
    for (uint i = 0; i < max; ++i) {
        if(i!=0){
            *(((uint64_t *)aPP) + i) = 0;
        }
    }
    // basic_header: LSN, PageIndex, PartitionId, Version, unused, unused
    basic_header_t lBH = {aLSN, aPageIndex, aPID, 1, 0, 0};
    fsm_header_t lHeader = {aNoPages, 0, lBH};  
    // ######## why 0 nextSegment? 0 is invalid, should be correct this way
    *((fsm_header_t *)(aPP + (max * 8))) = lHeader;
}

int InterpreterFSM::getFreePage(const PageStatus aPageStatus) {
    uint i = 0;
    while (i < _header->_noPages) {
        PageStatus lPageStatus = getPageStatus(i);
        // if fits on page
        bool fits = 0;
        PageStatus max = PageStatus::kNoBuckets;
        if (static_cast<int>(lPageStatus) + static_cast<int>(aPageStatus) <= static_cast<int>(max)) {
            fits = true;
        }
        if (fits) {
            lPageStatus = static_cast<PageStatus>(static_cast<int>(aPageStatus) + static_cast<int>(aPageStatus));
            changePageStatus(i, lPageStatus);
            return i;
        }
        ++i;
    }
    if (i < (_pageSize - sizeof(fsm_header_t)) * 2) { // add new page to segment
        changePageStatus(i, aPageStatus);
        _header->_noPages++;
        return i;
    } else {
        return -1; // no free space on this fsm, load or create next
    }

    // search for page with sufficient free space
    // calculate new occupation
    // change status
}

void InterpreterFSM::changePageStatus(const uint aPageNo, const PageStatus aStatus) {
    uint8_t *currByte = ((uint8_t *)_pp + aPageNo / 2);
    uint8_t lStatus = static_cast<uint8_t>(aStatus);
    if (aPageNo % 2 == 0) {
        *currByte &= ~15;
        *currByte |= lStatus;
    } else {
        lStatus <<= 4;
        *currByte &= 15;
        *currByte |= lStatus;
    }
}

PageStatus InterpreterFSM::getPageStatus(const uint aPageNo) {
    uint8_t currByte = *((uint8_t *)_pp + aPageNo / 2);
    if (aPageNo % 2 == 0) {
        currByte &= 15;
        return static_cast<PageStatus>(currByte);
    } else {
        currByte >>= 4;
        return static_cast<PageStatus>(currByte);
    }
}


PageStatus InterpreterFSM::calcPageStatus(const uint aSizeWithoutOverhead, const uint aNoBytes)
{
    if (aSizeWithoutOverhead < aNoBytes) {
        return PageStatus::kNoType;
    }
    const uint lBucketSize = std::floor(aSizeWithoutOverhead / 16.0); // remove magic number, (numb buckets)
    const uint lBucketNo = std::ceil(aNoBytes / (double)lBucketSize);
    return (lBucketNo < (uint)PageStatus::kNoBuckets) ? static_cast<PageStatus>(lBucketNo) : PageStatus::kNoType;
}
