#include "interpreter_sp.hh"

bool SP_Interpreter::_pageSizeSet = false;
size_t SP_Interpreter::_pageSize = 0;

void SP_Interpreter::setPageSize(const size_t aPageSize)
{
	if(!_pageSizeSet)
	{
		_pageSizeSet = !_pageSizeSet;
		_pageSize = aPageSize;
	}
	else
	{
		std::cerr << "ERROR: Page size can only be set once" << std::endl;
	}
}

SP_Interpreter::SP_Interpreter() : _pp(NULL), _header(NULL), _slots(0) 
{}

void SP_Interpreter::detach() 
{
	_pp     = 0;
	_header = 0;
	_slots  = 0;
}

void SP_Interpreter::initNewPage(byte* aPP) 
{
	if(aPP)
	{
		attach(aPP);
		header()->_noRecords = 0;
		header()->_freeSpace = (_pageSize - sizeof(sp_header_t));
		header()->_nextFreeRecord = 0;
		header()->_nextFreeSpace = 0;
	}
}

/* Give record size as parameter, determine where to write the record and return location as a pointer */
byte* SP_Interpreter::addNewRecord(const uint aRecordSize)
{
	const uint lRecordSize = ((aRecordSize + 7) & ~(uint) 0x07); // adjust for 8 byte alignment
	const uint lTotalSize = lRecordSize + sizeof(slot_t);        // add space for one new slot 

	byte* lResultRecord = 0;

	if(lTotalSize <= freeSpace()) 
	{
		lResultRecord = pagePtr() + header()->_nextFreeRecord;
                //wie viel platz genau da?
		header()->_nextFreeRecord += lRecordSize;               // remember pointer to next free record
		header()->_freeSpace -= lTotalSize;
		slot(noRecords())._offset = lResultRecord - pagePtr();  // store offset of new record in slot
                //rest des slots setzen
		header()->_noRecords += 1;
	}
	return lResultRecord;
}

