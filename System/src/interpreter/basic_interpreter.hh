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
#include "infra/header_structs.hh"

class BasicInterpreter
{
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
		void init(byte* aPP, uint64_t aLSN, uint32_t aPageIndex, uint8_t aPID);
		
	public:
		inline byte* 			pagePtr()	{ return _pp; }
		inline basic_header_t* 	header()	{ return _header; }
		inline uint16_t 		getPageSize(){ return _pageSize; } //do we need this function? do clients need to call this?
		inline uint32_t 		getPageIndex(){ return header()->_pageIndex; } //do we need this function? do clients need to call this?

	private:
    	inline basic_header_t* get_hdr_ptr(){ return (basic_header_t*)(_pp + _pageSize - sizeof(basic_header_t)); }

	private:
		byte* _pp;
		basic_header_t* _header;
		uint16_t _pageSize; //4096 bytes

};

void BasicInterpreter::attach(byte* aPP)
{
	_pp = aPP;
	_header = get_hdr_ptr();
	_pageSize = 4096; //atm hard coded
}


#endif