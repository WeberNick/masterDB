/**
 *  @file    header_structs.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A collection of different header types.
 *  @bugs    Currently no bugs known
 *  @todos   Change all mentions of blocks to pages. This involves further updates in the
 *  @section TBD
 */
#pragma once

#include <cstdint>

/* A basic header which is part of every page */
struct basic_header_t {
    uint64_t _LSN;       // Needed for recovery
    uint32_t _pageIndex; // Page index inside the partition
    uint8_t _PID;        // The ID of the partition this page is stored in
    uint8_t _version;    // Basic header version
    uint8_t _unused1;
    uint8_t _unused2;
};

/* A header for the free space indicator page */
struct fsip_header_t {
    uint32_t _freeBlocksCount; // Number of free pages in the managed part (numer of 0s)
    uint32_t _nextFreePage;    // index of the next 0 (indicating a free Block)
    uint32_t _managedPages;    // how many valid bits on fsip
    uint8_t _version;          // FSIP header version
    uint8_t _unused1;
    uint8_t _unused2;
    uint8_t _unused3;
    basic_header_t _basicHeader; // The basic header
};

/* A header for the free space management page of a segment */
struct fsm_header_t {
    uint32_t _noPages;           // number of Pages this FSM page handles
    uint32_t _nextFSM;           // pageIndex of the next FSM of this very segment
    basic_header_t _basicHeader; // The basic header
};

/* A header for a slotted page */
struct sp_header_t 
{
    uint16_t _noRecords;      // number of records stored on this page
    uint16_t _freeSpace;      // total number of free bytes
    uint16_t _nextFreeSpace;  // pointer to first free space on page
    uint8_t _unused1;
    uint8_t _unused2;
    basic_header_t _basicHeader; // The basic header
};

/* A header for the segment manager */
struct segment_index_header_t {
    uint32_t _nextIndexPage; // index to the next index page inside this partition, is invalid if set to 0
    uint16_t _noSegments;    // number of managed segments on this physical page only. May be larger for the segment manager
    uint8_t _version;        // Segment-index header version
    uint8_t _unused;
    basic_header_t _basicHeader; // The basic header
};

/* A header for a segment */
struct segment_page_header_t {
    uint16_t _maxSize;  // Max number of pages managed by this segment
    uint16_t _currSize; // Current number of pages managed by this segment
    uint16_t _segID;
    uint8_t _version; // Segment-page header version
    uint8_t _unused1;
    basic_header_t _basicHeader; // The basic header
};

/* A header for a fsm segment */
struct segment_fsm_header_t {
    uint32_t _currSize;      // Current number of pages managed by this segment on this page only
    uint32_t _firstFSM;      // physical index of the first FSM. Others are pointed at from there on
    uint32_t _nextIndexPage; // if segment has more than one index page, this is a physical index. else this is 0
    uint16_t _segID;
    uint8_t _version;
    uint8_t _unused;
    basic_header_t _basicHeader;
};

/* A header for a nsm segment (currently the same as segment_fsm_header_t) */
struct segment_fsm_sp_header_t {
    uint32_t _currSize;      // Current number of pages managed by this segment on this page only
    uint32_t _firstFSM;      // physical index of the first FSM. Others are pointed at from there on
    uint32_t _nextIndexPage; // if segment has more than one index page, this is a physical index. else this is 0
    uint16_t _segID;
    uint8_t _version;
    uint8_t _unused;
    basic_header_t _basicHeader;
};
