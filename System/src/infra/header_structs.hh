#ifndef HEADER_STRUCTS_HH
#define HEADER_STRUCTS_HH

#include <cstdint>

struct basic_header_t
{
	uint64_t _LSN; 
	uint32_t _pageIndex; //offset from the start of the partition
	uint8_t _PID; //partition ID
	uint8_t _version;
	uint8_t _unused1;
	uint8_t _unused2;
};

struct fsip_header_t
{
	basic_header_t _basicHeader;
	uint32_t _freeBlocksCount;	// number of free Blocks in the managed part (numer of 0s)
	uint32_t _nextFreeBlock;	// index of the next 0 (indicating a free Block)
	uint32_t _managedBlocks;	// index of the next 0 (indicating a free Block)
	uint8_t _version;
	uint8_t _unused1;
	uint8_t _unused2;
	uint8_t _unused3;
};

struct segment_index_header_t
{
	basic_header_t _basicHeader;
	uint32_t _nextIndexPage;
	uint16_t _noSegments;
	uint8_t _version;
	uint8_t _unused;
};

struct segment_page_header_t
{
	basic_header_t _basicHeader;
	uint16_t _maxSize;
	uint16_t _currSize;
	uint8_t _version;
	uint8_t _unused1;
	uint8_t _unused2;
	uint8_t _unused3;
};

//NSM Header etc follow





#endif