#ifndef CONFIG_HH
#define CONFIG_HH

#include <sys/stat.h>
#include <cstdlib>

const std::size_t 	C_PARTITION_SIZE = 2048000000; 	// 2GB = 500,000 * 4096
const std::size_t 	C_PAGE_SIZE = 4096;				// Page size of 4096 Bytes
const std::size_t 	C_GROWTH_INDICATOR = 0;
const mode_t 		C_ACCESS_RIGHTS = 0644;

#endif