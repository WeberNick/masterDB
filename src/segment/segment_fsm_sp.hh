/**
 * @file    segment_fsm_sp.hh
 * @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 * @brief   Class implementing a Segment with Free Space Management for N-ary Storage Model (SP)
 * @bugs    Currently no bugs known.
 * @todos   -are all changes correct?
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/segment_t.hh"
#include "../infra/header_structs.hh"
#include "../buffer/buf_cntrl_block.hh"
#include "../buffer/buf_mngr.hh"
#include "../interpreter/interpreter_sp.hh"
#include "segment_base.hh"
#include "segment_fsm.hh"

#include <vector>
#include <algorithm>
#include <cstring>

class SegmentFSM_SP : public SegmentFSM
{
  private:
    friend class SegmentManager;
    explicit SegmentFSM_SP() = delete;
    explicit SegmentFSM_SP(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
    explicit SegmentFSM_SP(PartitionBase& aPartition, const CB& aControlBlock);
    explicit SegmentFSM_SP(const SegmentFSM_SP&) = delete;
    explicit SegmentFSM_SP(SegmentFSM_SP&&) = delete;
    SegmentFSM_SP& operator=(const SegmentFSM_SP&) = delete;
    SegmentFSM_SP& operator=(SegmentFSM_SP&&) = delete;
    ~SegmentFSM_SP() = default;

  public:
    template<typename Tuple_T>
    TID insertTuple(const Tuple_T& aTuple);
    TID insertTuple(byte* aTuple, const uint aTupleSize);

    template<typename Tuple_T>
    tid_vt insertTuples(const std::vector<Tuple_T>& aTupleVector);
    tid_vt insertTuples(const byte_vpt& aTuples, const uint aTupleSize);

    template<typename Tuple_T>
    Tuple_T getTuple(const TID& aTID);
    tid_vt scan();

    PID getFreePage(uint aNoOfBytes, bool& emptyfix);

    int getMaxFreeBytes() noexcept { return getPageSize() - sizeof(segment_fsm_sp_header_t) -sizeof(sp_header_t);}
    void loadSegmentUnbuffered(const uint32_t aPageIndex) ;
    void readPageUnbuffered(uint aPageNo, byte* aPageBuffer, uint aBufferSize);   

  public:
    const tid_vt& getTIDs() const noexcept { return _tids; }
    const tid_vt& getTIDs() noexcept { return _tids; }

    protected:
	    void erase() override;

    private:
        template<typename Tuple_T>
        void insertTuplesSub(const std::vector<Tuple_T>& aTupleVector, size_t aStart,size_t aSize, tid_vt& result);

    private:
        tid_vt _tids;
};

template<typename Tuple_T>
TID SegmentFSM_SP::insertTuple(const Tuple_T& aTuple)
{
    TRACE("Trying to insert tuple : " + aTuple.to_string());
	// get page with enough space for the tuple and load it into memory
	bool emptyfix = false;
	PID lPID = getFreePage(aTuple.size(), emptyfix);
	BCB* lBCB;
	if(emptyfix){//the page is new, use different command on buffer
		lBCB = _bufMan.emptyfix(lPID);
	}
	else{
		lBCB = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE); 
	}
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	//if the page is new, it has to be initialised first.
	if(emptyfix){
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	// if enough space is free on nsm page, the pointer will point to location on page where to insert tuple
	auto [tplPtr, tplNo] = lInterpreter.addNewRecord(aTuple.size()); //C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
    const TID resultTID = {lPID.pageNo(), tplNo};
	
	if(!tplPtr) // If true, not enough free space on nsm page => getFreePage buggy
	{
		const std::string lErrMsg("Not enough free space on nsm page.");
        TRACE(lErrMsg);
        throw NSMException(FLF, lErrMsg);
	}
    aTuple.toDisk(tplPtr);
	lInterpreter.detach();
	lBCB->setModified(true);
	_bufMan.unfix(lBCB);
    TRACE("Inserted tuple successfully.");
    _tids.push_back(resultTID);
    return resultTID;
}

//outer function
template<typename Tuple_T>
tid_vt SegmentFSM_SP::insertTuples(const std::vector<Tuple_T>& aTupleVector){
    tid_vt result;
    insertTuplesSub(aTupleVector, 0,aTupleVector.size(),result);
    _tids.insert(_tids.cend(), result.cbegin(), result.cend());
    return result;
}

//inner function
template<typename Tuple_T>
void SegmentFSM_SP::insertTuplesSub(const std::vector<Tuple_T>& aTupleVector, size_t aStart,size_t aSize, tid_vt& result){
    TRACE("trying to insert many Tuples");
	// get page with enough space for the tuples and load it into memory
    if((aStart<0) || (aStart + aSize > aTupleVector.size())){
        TRACE("invalid boundaries: aStart: "+std::to_string(aStart)+" aSize: "+std::to_string(aSize)+" vectorSize: "+std::to_string(aTupleVector.size()));
        throw InvalidArgumentException(FLF,"aStart and aSize are not set correctly");
    }

    bool emptyfix = false;
    size_t lTotalSize = 0;
    for (size_t i = 0; i < aSize; ++i){
        //slotted page is 8byte aligned, allign tuple size
        lTotalSize += std::ceil(((double) aTupleVector.at(aStart + i).size() )/8.0)*8;
        lTotalSize += sizeof(InterpreterSP::slot_t);
    }
    PID lPID;
    try{
	    lPID = SegmentFSM::getFreePage(lTotalSize, emptyfix, sizeof(sp_header_t));
    }
    //exception handling buggy, will be fixed tomorrow
    catch(NSMException ex){
        //lTotalSize is bigger than size of a page. 
        //Now we try to approx the bin packing problem (NP hard) by a greedy algorithm...
        uint lPageSize = std::ceil(((getPageSize() - sizeof(sp_header_t))/16)*15);
        uint lPartSize;
        uint begin = 0;
        uint end = 0;
        while(begin < aSize){
            lPartSize = 0;
            while ((lPartSize < lPageSize) & (end < aSize)){
                //slotted page is 8byte aligned, allign tuple size
                lPartSize += std::ceil(((double) aTupleVector.at(aStart + end).size() )/8.0)*8;
                lPartSize += sizeof(InterpreterSP::slot_t);
                ++end;
            }
            if(lPartSize > lPageSize){
                end-=1;
            }
            TRACE("bin: Tuples from "+std::to_string(begin)+" to "+std::to_string(end));
            insertTuplesSub(aTupleVector, aStart + begin, end-begin ,result);
            TRACE("partial insert completed");
            begin = end;

        }
        return;
    }
	BCB* lBCB;
    //fix page, if it is new, use different command on buffer
	if(emptyfix){
		lBCB = _bufMan.emptyfix(lPID);
	}
	else{
		lBCB = _bufMan.fix(lPID,  LOCK_MODE::kEXCLUSIVE); 
	}
    //get iterator to PID in page vector
    auto it = std::find_if(_pages.begin(), _pages.end(), [&lPID] (const auto& elem) { return elem.first == lPID; }); 
    if(it != _pages.end())
    {
        TRACE("## Page found! Assign BCB Pointer to _pages vector");
        it->second = lBCB;
    }
    else
    {
        #pragma message ("TODO: @segment guys: same story as usual, can this be deleted? bug ever occured? Think about how this can happen!")
        TRACE("## This should not be printed");
        //terminate and find bug
        throw ReturnException(FLF);
    }
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	//if the page is new, it has to be initialised first.
	if(emptyfix){
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	    
	for (size_t i = 0; i < aSize; ++i){
        // if enough space is free on nsm page, the pointer will point to location on page where to insert a tuple
        auto [tplPtr, tplNo] = lInterpreter.addNewRecord(aTupleVector.at(aStart + i).size()); //C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
        const TID resultTID = {lPID.pageNo(), tplNo};
	
	    if(!tplPtr) // If true, not enough free space on nsm page => getFreePage buggy
	    {
		    const std::string lErrMsg("Not enough free space on nsm page.");
            TRACE(lErrMsg);
            throw NSMException(FLF, lErrMsg);
	    }
        aTupleVector.at(aStart + i).toDisk(tplPtr);
		result.push_back(resultTID);
        TRACE("inserted Tuple");
	}
    
	lInterpreter.detach();
	//lBCB->setModified(true);
	//_bufMan.unfix(lBCB);
    TRACE("page to release: "+std::to_string(it-_pages.begin()));
    releasePage(it-_pages.begin(),true);
    TRACE("Inserted tuples successfully.");
    return;
}

//not fully tested. used as convinience functionality
template<typename Tuple_T>
Tuple_T SegmentFSM_SP::getTuple(const TID& aTID)
{
    TRACE("Looking for tuple with TID " + aTID.to_string());
    Tuple_T result;
    const auto it = std::find_if(_pages.begin(), _pages.end(), [&aTID] (const auto& elem) { return elem.first.pageNo() == aTID.pageNo(); }); //get iterator to PID in page vector
    if(it != _pages.cend())
    { 
        const size_t index = it - _pages.cbegin();
        byte* lPagePtr = getPage(index, LOCK_MODE::kSHARED);
        InterpreterSP lInterpreter;
        lInterpreter.attach(lPagePtr);
        byte* lTuplePtr = lInterpreter.getRecord(aTID.tupleNo());
        if(lTuplePtr)
        { 
            result.toMemory(lTuplePtr); 
            #pragma message ("TODO: @Jonas is this comment still valid? Was this a bug elsewhere? Is it fixed?")
            releasePage(index); //crashed the buffer...
            return result;
        }
        releasePage(index); //crashed the buffer...
    }
    throw TupleNotFoundOrInvalidException(FLF);
}
/*
template<typename Tuple_T>
tid_vt SegmentFSM_SP::insertTuples(const std::vector<Tuple_T>& aTupleVector){
    TRACE("trying to insert many Tuples");
	// get page with enough space for the tuples and load it into memory
	tid_vt result;
    bool emptyfix = false;
    size_t lTotalSize = 0;
    for (auto& iter : aTupleVector){
        //slotted page is 8byte aligned, allign tuple size
        lTotalSize += std::ceil(((double) iter.size() )/8.0)*8;
        lTotalSize += sizeof(InterpreterSP::slot_t);
    }
    PID lPID;
    try{
	    lPID = SegmentFSM::getFreePage(lTotalSize, emptyfix, sizeof(sp_header_t));
    }
    //exception handling buggy, will be fixed tomorrow
    catch(NSMException ex){
        //lTotalSize is bigger than size of a page. 
        //Now we try to approx the bin packing problem (NP hard) by a greedy algorithm...
        uint lPageSize = std::ceil(((getPageSize() - sizeof(sp_header_t))/16)*15);
        uint lPartSize;
        uint start = 0;
        uint end = 0;
        while(start < aTupleVector.size()){
            lPartSize = 0;
            start = end;
            while ((lPartSize < lPageSize) & (end < aTupleVector.size())){
                //slotted page is 8byte aligned, allign tuple size
                lPartSize += std::ceil(((double) aTupleVector.at(end).size() )/8.0)*8;
                lPartSize += sizeof(InterpreterSP::slot_t);
                ++end;
            }
            if(lPartSize > lPageSize){
                end-=1;
            }
            TRACE("bin: Tuples from "+std::to_string(start)+" to "+std::to_string(end));
            std::vector<Tuple_T> temp (aTupleVector.begin()+start, aTupleVector.begin()+end);
            tid_vt tempRes = insertTuples(temp);
            TRACE("partial insert completed");
            result.insert( result.end(), tempRes.begin(), tempRes.end() );
        }
        return result;
    }
	BCB* lBCB;
    //fix page, if it is new, use different command on buffer
	if(emptyfix){
		lBCB = _bufMan.emptyfix(lPID);
	}
	else{
		lBCB = _bufMan.fix(lPID,  LOCK_MODE::kEXCLUSIVE); 
	}
    //get iterator to PID in page vector
    auto it = std::find_if(_pages.begin(), _pages.end(), [lPID] (const std::pair<PID, BCB*>& elem) { return elem.first == lPID; }); 
    if(it != _pages.end())
    {
        TRACE("## Page found! Assign BCB Pointer to _pages vector");
        it->second = lBCB;
    }
    else
    {
        TRACE("## This should not be printed");
        //terminate and find bug
        throw ReturnException(FLF);
    }
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	//if the page is new, it has to be initialised first.
	if(emptyfix){
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	    
	for(auto& lTuple : aTupleVector)
	{
        // if enough space is free on nsm page, the pointer will point to location on page where to insert a tuple
        auto [tplPtr, tplNo] = lInterpreter.addNewRecord(lTuple.size()); //C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
        const TID resultTID = {lPID.pageNo(), tplNo};
	
	    if(!tplPtr) // If true, not enough free space on nsm page => getFreePage buggy
	    {
		    const std::string lErrMsg("Not enough free space on nsm page.");
            TRACE(lErrMsg);
            throw NSMException(FLF, lErrMsg);
	    }
        lTuple.toDisk(tplPtr);
		result.push_back(resultTID);
        TRACE("inserted Tuple");
	}
    
	lInterpreter.detach();
	lBCB->setModified(true);
	_bufMan.unfix(lBCB);
    TRACE("Inserted tuples successfully.");
    return result;
}*/
