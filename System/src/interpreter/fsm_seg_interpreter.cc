#include "fsm_seg_interpreter.hh"

bool FSMInterpreter::_pageSizeSet = false;
uint16_t FSMInterpreter::_pageSize = 0;

void FSMInterpreter::setPageSize(const uint16_t aPageSize) {
    if (!_pageSizeSet) {
        _pageSizeSet = !_pageSizeSet;
        _pageSize = aPageSize;
    } else {
        std::cerr << "ERROR: Page size can only be set once" << std::endl;
    }
}

FSMInterpreter::FSMInterpreter() : _pp(NULL), _header(NULL) {}

FSMInterpreter::~FSMInterpreter() {}

void FSMInterpreter::detach() {
    _pp = NULL;
    _header = NULL;
}

void FSMInterpreter::initNewFSM(byte *aPP, const uint64_t aLSN, const uint32_t aPageIndex, const uint8_t aPID,
                                const uint32_t aNoPages) {
    // alles 0, header
    uint max = (_pageSize - sizeof(fsm_header_t)) / 8;
    for (uint i = 0; i < max; ++i) {
        *(((uint64_t *)aPP) + i) = 0;
    }
    // basic_header: LSN, PageIndex, PartitionId, Version, unused
    basic_header_t lBH = {aLSN, aPageIndex, aPID, 1, 0};
    fsm_header_t lHeader = {aNoPages, 0, lBH};
    *((fsm_header_t *)(aPP + (max * 8))) = lHeader;
}

int FSMInterpreter::getFreePage(const uint64_t aLSN, SegmentPageStatus aPageStatus) {
    // uint max = (_pageSize-sizeof(fsm_header_t))*2;
    uint i = 0;
    while (i < _header->_noPages) {
        SegmentPageStatus lPageStatus = getPageStatus(i);
        // if fits on page
        bool fits = 0;
        if (lPageStatus + aPageStatus <= 4) {
            fits = true;
        }
        if (fits) {
            aPageStatus = static_cast<SegmentPageStatus>(aPageStatus + lPageStatus);
            changePageStatus(i, aPageStatus);
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

void FSMInterpreter::changePageStatus(const uint aPageNo, SegmentPageStatus aStatus) {
    uint8_t *currByte = ((uint8_t *)_pp + aPageNo / 2);
    uint8_t lStatus = aStatus;
    if (aPageNo % 2 == 0) {
        *currByte &= ~15;
        *currByte |= lStatus;
    } else {
        lStatus <<= 4;
        *currByte &= 15;
        *currByte |= lStatus;
    }
}

SegmentPageStatus FSMInterpreter::getPageStatus(const uint aPageNo) {
    uint8_t currByte = *((uint8_t *)_pp + aPageNo / 2);
    if (aPageNo % 2 == 0) {
        currByte &= 15;
        return static_cast<SegmentPageStatus>(currByte);
    } else {
        currByte >>= 4;
        return static_cast<SegmentPageStatus>(currByte);
    }
}
