#include "interpreter_sp.hh"

size_t InterpreterSP::_pageSize = 4096;

void InterpreterSP::setPageSize(const size_t aPageSize) noexcept {
    _pageSize = aPageSize;
}

InterpreterSP::InterpreterSP() : _pp(nullptr), _header(nullptr), _slots(nullptr) 
{}

void InterpreterSP::detach() noexcept
{
	_pp     = nullptr;
	_header = nullptr;
	_slots  = nullptr;
}

void InterpreterSP::initNewPage(byte* aPP) noexcept 
{
	if(aPP)
	{
		attach(aPP);
		header()->_noRecords = 0;
		header()->_freeSpace = (_pageSize - sizeof(sp_header_t));
		header()->_nextFreeSpace = 0;
		header()->_unused1 = 0;
		header()->_unused2 = 0;
		
		aPP += header()->_nextFreeSpace;
		*((freeSpaceList_t*) aPP) = freeSpaceList_t{0, static_cast<uint16_t>(_pageSize - sizeof(sp_header_t) - sizeof(freeSpaceList_t)) };
	}
}

/* Give record size as parameter, determine where to write the record and return location as a pointer */
byte* InterpreterSP::addNewRecord(const uint aRecordSize) noexcept
{
	const uint lRecordSize = ((aRecordSize + 7) & ~(uint) 0x07); // adjust for 8 byte alignment
	const uint lTotalSize = lRecordSize + sizeof(slot_t);        // add space for one new slot 

	byte* lResultRecord = nullptr;

	if(lTotalSize <= freeSpace()) 
	{
		lResultRecord = pagePtr() + header()->_nextFreeSpace;
                //wie viel platz genau da?
		header()->_nextFreeSpace += lRecordSize;               // remember pointer to next free record
		header()->_freeSpace -= lTotalSize;
		slot(noRecords())._offset = lResultRecord - pagePtr();  // store offset of new record in slot
		slot(noRecords())._size =lRecordSize;
		slot(noRecords())._status=1;
                //rest des slots setzen
		header()->_noRecords += 1;
	}
/*
	if(lTotalSize <= freeSpace()) 
	{
		lResultRecord = pagePtr() + header()->_nextFreeSpace;
		byte* lPrevPtr = 0;
		//while there is not enough space and there are more elements in the free space list
		while( ((freeSpaceList_t*) lResultRecord)->_size < lRecordSize & !(((freeSpaceList_t*) lResultRecord)->_offset==0)){
			//look up the next entry of the free space list
			lPrevPtr = lResultRecord;
			lResultRecord = pagePtr() + ((freeSpaceList_t*) lResultRecord)->_offset;
		}
		//if not,  leave position out
		if(((freeSpaceList_t*) lResultRecord)->_size < (lRecordSize+sizeof(freeSpaceList_t))){
			if(lPrevPtr==0){ // if one has to change the header directly
				header()->_nextFreeSpace = ((freeSpaceList_t*) lResultRecord)->_offset;
			}
			else{
				((freeSpaceList_t*) lPrevPtr)->_offset= ((freeSpaceList_t*) lResultRecord)->_offset;
			}
		}
		else{
		//move pointer behind record if there is enough space
		freeSpaceList_t temp = *(freeSpaceList_t*) lResultRecord;
		uint16_t lNewFSLPos = lResultRecord+lRecordSize-pagePtr();
		temp._size-=lRecordSize;
		*((freeSpaceList_t*) pagePtr() + lNewFSLPos)=temp;
		//edit previous pointer
			if(lPrevPtr==0){ // if one has to change the header directly
				header()->_nextFreeSpace = lNewFSLPos;
			}
			else{
				((freeSpaceList_t*) lPrevPtr)->_offset = lNewFSLPos;
			}
		}

		header()->_freeSpace -= lTotalSize;
		slot(noRecords())._offset = lResultRecord - pagePtr();  // store offset of new record in slot
		slot(noRecords())._size =lRecordSize;
		slot(noRecords())._status=1;
                //rest des slots setzen
		header()->_noRecords += 1;
	}*/
	return lResultRecord;
}

//just mark deleted
int InterpreterSP::deleteRecordSoft (uint16_t aRecordNo) noexcept {
	slot(aRecordNo)._status=0;
	return 1;
}
//actually delete it
int InterpreterSP::deleteRecordHard (uint16_t aRecordNo) noexcept {
	//TODO

	//put free in front of Free Space List by
		//check if there is list entry behind this record
			//if yes, move this one at beginning and increment size
			//if no, create new entry, put _header._nextFreeSpace as new offset
		//do something with the slot...
		//return 1
	return -1;

}
byte* InterpreterSP::getRecord(uint aRecordNo) noexcept {
	if(aRecordNo >= noRecords()) { 
		return nullptr;
	}
	else{
		if(slot(aRecordNo)._status==0){
			return nullptr;
		}
		else{
			return _pp+slot(aRecordNo)._offset;
		}
	}
}
