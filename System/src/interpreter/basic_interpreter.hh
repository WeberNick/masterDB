/**
 *  @file    basic_interpreter.hh
 *  @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de), Jonas Thietke (jthietke@mail.uni-mannheim.de)
 *  @brief   A class implementing a basic, content independent, block interpreter
 *  @bugs    Currently no bugs known, but there are supposed to be a lot
 *  @todos   probably a lot
 *  @section TBD
 */
#ifndef BASIC_INTERPRETER_HH
#define BASIC_INTERPRETER_HH

#include "infra/types.hh"

class BasicInterpreter
{
	public:
		struct basic_header_t{
			uint64_t _LSN; 
			uint32_t _offset; //offset from the start of the partition
			uint8_t _PID; //partition ID
			uint8_t _padding1;
			uint8_t _padding2;
			uint8_t _padding3;
		};

	public:
		BasicInterpreter();
		/* If CC and AO are needed, implement them correctly */
		BasicInterpreter(const BasicInterpreter& aInterpreter) = delete;
		BasicInterpreter& operator=(const BasicInterpreter& aInterpreter) = delete;
		~BasicInterpreter();


	public:
		inline void attach(byte* aPP);
		void detach();

	public:
		void init(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID);
		
	public:
		inline byte* 			pagePtr()	{ return _pp; }
		inline basic_header_t* 	header()	{ return _header; }
		inline size_t			getHeaderSize(){ return sizeof(basic_header_t); }
		inline uint16_t 		getBlockSize(){ return _blockSize; } //do we need this function? do clients need to call this?
		inline uint32_t 		getPartitionOffset(){ return header()->_offset; } //do we need this function? do clients need to call this?

	private:
    	inline basic_header_t* get_hdr_ptr(){ return (basic_header_t*)(_pp + _blockSize - sizeof(basic_header_t)); }

	private:
		byte* _pp;
		basic_header_t* _header;
		uint16_t _blockSize; //4096 bytes

};

void BasicInterpreter::attach(byte* aPP)
{
	_pp = aPP;
	_header = get_hdr_ptr();
	_blockSize = 4096; //atm hard coded
}


#endif