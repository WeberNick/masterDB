/**
 *  @file	basic_interpreter.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), 
			Jonas Thietke (jthietke@mail.uni-mannheim.de),
			Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 *  @brief	A class implementing a Free Space Indicator Page (FSIP) interpreter for little Endian
 *  @bugs	might not work for big Endian
 *  @todos	Remove noManagedPages() and implement it correctly
 *  @section TBD
 */
#ifndef FSIP_INTERPRETER_HH
#define FSIP_INTERPRETER_HH

#include "infra/types.hh"
#include "infra/header_structs.hh"
#include "infra/bit_intrinsics.hh"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

class FSIPInterpreter
{
	public:
		/* standard constructor */
		FSIPInterpreter();
		/* If CC and AO are needed, implement them correctly */
		FSIPInterpreter(const FSIPInterpreter& aInterpreter) = delete;
		/* specifies the assign operator of a intermediate buffer with delete */
		FSIPInterpreter& operator=(const FSIPInterpreter& aInterpreter) = delete;
		/* standard destructor */						
		~FSIPInterpreter();

	public:
		/*	Set page size */
		static void setPageSize(const uint16_t aPageSize);

	public:
		/*	set the page pointer and header */
		inline void  attach(byte* aPP);
		/*	unset the page pointer and header */
		void  detach();

	public:
		/**
		 *	@brief	initialize the FSIP through setting all bits to 0 and the header
		 *
		 *	@param	aPP - Pointer to the start of the page
		 *	@param	aLSN - Log Sequence Number
		 *	@param	aOffset - Page index inside the partition
		 *	@param	aPID - The ID of the partition this page is stored in
		 *	@param	aNoBlocks - Number of stored Pages in FSIP
		 */
		void initNewFSIP(byte* aPP, const uint64_t aLSN, const uint32_t aOffset, const uint8_t aPID, const uint32_t aNoBlocks);

		/**
		 *	@brief	looks for the next free block in the FSIP and reserves the page
		 *
		 *	@param	aPP - Pointer to the start of the page
		 *	@param	aLSN - Log Sequence Number
		 *	@param	aOffset - Page index inside the partition
		 *
		 * 	@return either an offset to the free block or -1 if no free block was found
		 */
		int getNewPage(byte* aPP, const uint64_t aLSN, const uint8_t aPID);

		/**
		 *	@brief	reserve the page at the given index position
		 *
		 *	@param	aPageIndex - Page index inside the partition
		 *
		 * 	@return either 0 if successful or -1 if not successful
		 */
		int reservePage(const uint aPageIndex);

		/**
		 *	@brief	free the page at the given index position
		 *
		 *	@param	aPageIndex - Page index inside the partition
		 */
		void freePage(const uint aPageIndex);
	public:
		/* Getters*/
		inline byte* 			pagePtr()	{ return _pp; }
		inline fsip_header_t* 	header()	{ return _header; }
		inline uint 			noManagedPages() { return header()->_managedPages; } //Remove noManagedPages()
		inline uint getHeaderSize(){ return sizeof(fsip_header_t); }

	private:
		/* Getter*/
		inline fsip_header_t* get_hdr_ptr(){ return (fsip_header_t*)(_pp + _pageSize - sizeof(fsip_header_t)); }

	private:
		/**
		 *	@brief	get the Position of the next free page
		 *
		 * 	@return either the position if successful or 0 
		 */
		uint getNextFreePage();

		/*	create the page with the given index in a file for debugging purposes */
		void debug(const uint aPageIndex);

	private:
		/*	pointer to the beginning of the page */
		byte* _pp; 
		/*	FSIP Header */
		fsip_header_t* _header;
		/*	if page size is set or not */
		static bool _pageSizeSet;
		/*	full size of the page */
		static uint16_t _pageSize;

};

void FSIPInterpreter::attach(byte* aPP) 
{
	_pp = aPP;
	_header = get_hdr_ptr();
}




#endif