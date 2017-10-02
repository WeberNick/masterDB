/**
 *  @file    header_structs.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief   A collection of different header types.
 *  @bugs    Currently no bugs known
 *  @todos   Change all mentions of blocks to pages. This involves further updates in the 
 *  @section TBD
 */
#ifndef HEADER_STRUCTS_HH
#define HEADER_STRUCTS_HH

#include <cstdint>

/* A basic header which is part of every page */
struct basic_header_t
{
	uint64_t _LSN; //Needed for recovery
	uint32_t _pageIndex; //Page index inside the partition
	uint8_t _PID; //The ID of the partition this page is stored in
	uint8_t _version; //Basic header version
	uint8_t _unused1;
	uint8_t _unused2;
};

struct fsip_header_t
{
	basic_header_t _basicHeader; //The basic header
	uint32_t _freeBlocksCount; //Number of free pages in the managed part (numer of 0s)
	uint32_t _nextFreeBlock; // index of the next 0 (indicating a free Block)
	uint32_t _managedBlocks; // index of the next 0 (indicating a free Block)
	uint8_t _version; //FSIP header version
	uint8_t _unused1;
	uint8_t _unused2;
	uint8_t _unused3;
};

struct segment_index_header_t
{
	basic_header_t _basicHeader; //The basic header
	uint32_t _nextIndexPage; //index to the next index page inside this partition
	uint16_t _noSegments; //number of managed segments
	uint8_t _version; //Segment-index header version
	uint8_t _unused;
};

struct segment_page_header_t
{
	basic_header_t _basicHeader; //The basic header
	uint16_t _maxSize; //Max number of pages managed by this segment
	uint16_t _currSize; //Current number of pages managed by this segment
	uint8_t _version; //Segment-page header version
	uint8_t _unused1;
	uint8_t _unused2;
	uint8_t _unused3;
};

//NSM Header etc follow





#endif