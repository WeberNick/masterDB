/**
 *  @file    basic_interpreter.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Jonas Thietke (jthietke@mail.uni-mannheim.de)
 *  @brief   A class implementing a Free Space Indicator Page (FSIP) interpreter
 *  @bugs    Currently no bugs known
 *  @todos   Write todos
 *  @section TBD
 */
#ifndef FSIP_INTERPRETER_HH
#define FSIP_INTERPRETER_HH

#include "infra/types.hh"
#include "basic_interpreter.hh"

class FSIPInterpreter
{
	public:
		FSIPInterpreter();
		/* If CC and AO are needed, implement them correctly */
		FSIPInterpreter(const FSIPInterpreter& aInterpreter) = delete;
		FSIPInterpreter& operator=(const FSIPInterpreter& aInterpreter) = delete;
		~FSIPInterpreter();

	public:
		inline void  attach(byte* aPP);
		void  detach();

	public:
		void initNewFSIP(byte* aPP, const uint64_t aLSN, const uint32_t aOffset, const uint8_t aPID, const uint32_t aNoBlocks);	//to implement

		/**
		 *	@brief	looks for the next free block in the FSIP
		 *
		 *	@param	aPP  reference to the intermediate buffer to read the tuples from
		 * 	@return either an offset to the free block or -1 if no free block was found
		 */
		const int getNewPage(byte* aPP, const uint64_t aLSN, const uint8_t aPID); //to implement
		void freePage(uint aPageIndex); //to implement

	public:
		inline byte* 			pagePtr()	{ return _pp; }
		inline FSIP_header_t* 	header()	{ return _header; }
		inline uint 			noManagedPages() { return header()->_managedBlocks; }

	private:
    	inline FSIP_header_t* get_hdr_ptr()
    	{ 
			BasicInterpreter bi; //better way?
			return (FSIP_header_t*)(_pp + _pageSize - sizeof(fsip_header_t));
		}


	private:
		byte* _pp;
		fsip_header_t* _header;
		uint16_t _pageSize; //4096 bytes

};

void FSIPInterpreter::attach(byte* aPP) 
{
	_pp = aPP;
	_pageSize = 4096; //atm hard coded
	_header = get_hdr_ptr();
}




#endif