#ifndef CONFIG_HH
#define CONFIG_HH

#include "infra/header_structs.hh"
#include <cstdlib>

constexpr std::size_t c_PartitionSizeInPages(){ return 500000; }	// 2GB = 500,000 * 4096
constexpr std::size_t c_PageSize(){ return 4096; }					// Page size of 4096 Bytes
constexpr std::size_t c_GrowthIndicator(){ return 1000; }
constexpr std::size_t c_PagesPerFSIP(){ return (c_PageSize() - sizeof(fsip_header_t)) / 8; }
constexpr std::size_t c_SegmentIndexPagePos(){ return 1; }			//the index inside a partition where to store the segment index page

#endif