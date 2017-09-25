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

class FSIPInterpreter
{
	//0: block is free, 1: block is occupied
	public:
		struct FSIP_header_t {
			uint32_t _freeBlocksCount;	// number of free Blocks in the managed part (numer of 0s)
			uint32_t _nextFreeBlock;	// index of the next 0 (indicating a free Block)
			uint32_t _managedBlocks;	// index of the next 0 (indicating a free Block)
			uint32_t _placeholder;	
		};
		FSIPInterpreter();
		~FSIPInterpreter();

	public:
		inline void  attach(byte* aPP);
		void  detach();	//to implement

	public:
		void initNewFSIP(const byte* aPP, const uint64_t aLSN, const uint32_t aOffset, const uint8_t aPID, const uint32_t aNoBlocks);	//to implement

		/**
		 *	@brief	looks for the next free block in the FSIP
		 *
		 *	@param	aPP  reference to the intermediate buffer to read the tuples from
		 * 	@return either an offset to the free block or -1 if no free block was found
		 */
		const int getNewBlock(byte* aPP); //to implement
		void freeBlock(uint aOffset); //to implement

	public:
		inline byte* 			pagePtr()	{ return _pp; }
		inline FSIP_header_t* 	header()	{ return _header; }
		inline uint 			noManagedBlocks() { return header()->_managedBlocks; }

	private:
    	inline FSIP_header_t* get_hdr_ptr(){ return 0; } //to implement


	private:
		byte* _pp;
		FSIP_header_t* _header;
		static uint16_t _headersize; //8 bytes //Note(Nick): kein attribut benötigt, einfach sizeof(FSIP_header_t)

};

void FSIPInterpreter::attach(byte* aPP) 
{
	_pp = aPP;
	_header = get_hdr_ptr();
}



#endif