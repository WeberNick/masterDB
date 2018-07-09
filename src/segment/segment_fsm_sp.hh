/**
 * @file    segment_fsm_sp.hh
 * @author  Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *          Jonas Thietke
 * @brief   Class implementing a Segment with Free Space Management for N-ary Storage Model (SP)
 * @bugs    Currently no bugs known.
 * @todos   -
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
    
        /**
        * @brief   Inserts all tuples of the vector into a segment. Can handle vector larger than the size of a page
        *          and can be therefore used as bulk insert. Tries to pack tuples as densely as possible.
        *
        * @param   aTupleVector vector containing all tuples to be inserted in form a of a class.
        * @return  vector of all TIDs inserted
        */
        template<typename Tuple_T>
        tid_vt insertTuples(const std::vector<Tuple_T>& aTupleVector);
        /**
        * @brief   Inserts all tuples of the vector into a segment. Calls insertTuple for every tuple provided.
        *          generally, the templated method should be used whenever possible.
        *
        * @param   aTuples     vector containing all tuples to be inserted in form of pointers
        * @param   aTupleSize  tuples are assumed to be fixed sized. This size holds for all.
        * @return  vector of all TIDs inserted
        */
        tid_vt insertTuples(const byte_vpt& aTuples, const uint aTupleSize);
    
        /**
        * @brief   loads a tuple to which the TID is provided.
        * 
        * @param   aTID   TID of tuple to be loaded
        * @return  the tuple is loaded into main memory and returned
        */
        template<typename Tuple_T>
        Tuple_T getTuple(const TID& aTID);
        /**
        * @brief   loads several tuple to which the TIDs are provided. Tries to unlock pages only when necessary.
        * 
        * @param   aTIDs   a vector of TIDs of tuple to be loaded
        * @return  a vector of tuples loaded into main memory
        */
        template<typename Tuple_T>
        std::vector<Tuple_T> getTuples(const tid_vt& aTIDs);
        /**
        * @brief   collects all valid TIDs of a segment.
        * 
        * @return  the TIDs to all valid tuples of a segment.
        */
        tid_vt scan();
    
        /**
        * @brief   gets a free page with enough space to store aNoOfBytes. Makes use of the getFreePage method
        *          of the SegmentFSM but uses the paraters for slotted pages
        * 
        * @param   aNoOfBytes  how many free bytes shall be requested?
        * @param   emptyfix    used internally as second return value. Will tell if page has been already initialised or not.
        * @return  the PID of a page having a sufficient amount of free space.
        */    
        PID getFreePage(uint aNoOfBytes, bool& emptyfix);

        int getMaxFreeBytes() noexcept { return getPageSize() - sizeof(segment_fsm_sp_header_t) -sizeof(sp_header_t);}
        /**
        * The unbuffered methods work like the buffered ones but use their own small buffer with the size of a page.
        * Are used during boot as the buffer manager is not working until everything is loaded.
        */
        void loadSegmentUnbuffered(const uint32_t aPageIndex) ;
        void readPageUnbuffered(uint aPageNo, byte* aPageBuffer, uint aBufferSize);   

    public:
        const tid_vt& getTIDs() const noexcept { return _tids; }
        const tid_vt& getTIDs() noexcept { return _tids; }

    protected:
        /**
        * @brief erases the segment. This does not destroy the object itself but frees all its pages.
        *
        */
	    void erase() override;

    private:
        template<typename Tuple_T>
        void insertTuplesSub(const std::vector<Tuple_T>& aTupleVector, size_t aStart, size_t aSize, tid_vt& result);

    private:
        tid_vt _tids; // vector of tuple ids contained in the segment
};

template<typename Tuple_T>
TID SegmentFSM_SP::insertTuple(const Tuple_T& aTuple)
{
    TRACE("Trying to insert tuple : " + aTuple.to_string());
	// get page with enough space for the tuple and load it into memory
	bool emptyfix = false;
	PID lPID = getFreePage(aTuple.size(), emptyfix);
	BCB* lBCB;
	if(emptyfix)
    { // the page is new, use different command on buffer
		lBCB = _bufMan.emptyfix(lPID);
	}
	else
    {
		lBCB = _bufMan.fix(lPID, LOCK_MODE::kEXCLUSIVE); 
	}
	//insert page into _pages data structure
    auto it = std::find_if(_pages.begin(), _pages.end(), [&lPID] (const auto& elem) { return elem.first == lPID; }); //get iterator to PID in page vector
    if(it != _pages.end())
    {
        TRACE("## Page found! Assign BCB Pointer to _pages vector");
        it->second = lBCB;
    }
 	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	// if the page is new, it has to be initialised first.
	if(emptyfix)
    {
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	// if enough space is free on nsm page, the pointer will point to location on page where to insert tuple
	auto [tplPtr, tplNo] = lInterpreter.addNewRecord(aTuple.size()); // C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
    const TID resultTID = {lPID.pageNo(), tplNo};
	
	if(!tplPtr) // If true, not enough free space on nsm page => invalid state of the system
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

// outer function
template<typename Tuple_T>
tid_vt SegmentFSM_SP::insertTuples(const std::vector<Tuple_T>& aTupleVector)
{
    tid_vt result;
    insertTuplesSub(aTupleVector, 0,aTupleVector.size(),result);
    _tids.insert(_tids.cend(), result.cbegin(), result.cend());
    return result;
}

// inner function
template<typename Tuple_T>
void SegmentFSM_SP::insertTuplesSub(const std::vector<Tuple_T>& aTupleVector, size_t aStart,size_t aSize, tid_vt& result)
{
    TRACE("trying to insert many Tuples");
	// get page with enough space for the tuples and load it into memory
    if((aStart<0) || (aStart + aSize > aTupleVector.size()))
    {
        TRACE("invalid boundaries: aStart: " + std::to_string(aStart) + " aSize: " + std::to_string(aSize) + " vectorSize: " + std::to_string(aTupleVector.size()));
        throw InvalidArgumentException(FLF,"aStart and aSize are not set correctly");
    }

    bool emptyfix = false;
    size_t lTotalSize = 0;
    for (size_t i = 0; i < aSize; ++i)
    {
        // slotted page is 8byte aligned, allign tuple size
        lTotalSize += std::ceil(((double) aTupleVector.at(aStart + i).size() )/8.0)*8;
        lTotalSize += sizeof(InterpreterSP::slot_t);
    }
    PID lPID;
    try
    {
	    lPID = SegmentFSM::getFreePage(lTotalSize, emptyfix, sizeof(sp_header_t));
    }
    catch(NSMException ex)
    {
        // lTotalSize is bigger than size of a page. 
        // Now we try to approx the bin packing problem (NP hard) by a greedy algorithm...
        const uint lNoBucks = toType(PageStatus::kMAX); // how many buckets does the fsm use? just to make code more readable
        const uint lPageSize = ((getPageSize() - sizeof(sp_header_t))/lNoBucks)*lNoBucks;
        uint lPartSize;
        uint begin = 0;
        uint end = 0;
        while(begin < aSize)
        {
            lPartSize = 0;
            while ((lPartSize < lPageSize) & (end < aSize))
            {
                // slotted page is 8byte aligned, allign tuple size
                lPartSize += std::ceil(((double) aTupleVector.at(aStart + end).size() )/8.0)*8;
                lPartSize += sizeof(InterpreterSP::slot_t);
                ++end;
            }
            if(lPartSize > lPageSize)
            {
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
    // fix page, if it is new, use different command on buffer
	if(emptyfix)
    {
		lBCB = _bufMan.emptyfix(lPID);
	}
	else
    {
		lBCB = _bufMan.fix(lPID,  LOCK_MODE::kEXCLUSIVE); 
	}
    // get iterator to PID in page vector
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
        // terminate and find bug
        throw ReturnException(FLF);
    }
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	// if the page is new, it has to be initialised first.
	if(emptyfix)
    {
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	    
	for (size_t i = 0; i < aSize; ++i)
    {
        // if enough space is free on nsm page, the pointer will point to location on page where to insert a tuple
        auto [tplPtr, tplNo] = lInterpreter.addNewRecord(aTupleVector.at(aStart + i).size()); // C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
        const TID resultTID = {lPID.pageNo(), tplNo};
	
	    if(!tplPtr) // If true, not enough free space on nsm page => some invalid state in system
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
	// lBCB->setModified(true);
	// _bufMan.unfix(lBCB);
    TRACE("page to release: " + std::to_string(it - _pages.begin()));
    releasePage(it - _pages.begin(),true);
    TRACE("Inserted tuples successfully.");
    return;
}

// not fully tested. used as convenience functionality
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
            releasePage(index); // crashed the buffer...
            return result;
        }
        releasePage(index); // crashed the buffer...
    }
    throw TupleNotFoundOrInvalidException(FLF);
}

template<typename Tuple_T>
std::vector<Tuple_T> SegmentFSM_SP::getTuples(const tid_vt& aTIDs)
{
    TRACE("Looking for a lot of tuples");
    std::vector<Tuple_T> result;
    result.reserve(aTIDs.size());
    Tuple_T t;

    size_t prev;
    size_t curr = INVALID;
    byte* lPagePtr;
    InterpreterSP lInterpreter;

    for(auto& tid : aTIDs)
    {
        TRACE("trying to get tuple "+tid.to_string());
        
        const auto it = std::find_if(_pages.begin(), _pages.end(), [&tid] (const auto& elem) { return elem.first.pageNo() == tid.pageNo(); }); //get iterator to PID in page vector
        if(it != _pages.cend())
        { 
            prev = curr;
            curr = it - _pages.cbegin();
            lPagePtr = getPage(curr, LOCK_MODE::kSHARED);
            
            lInterpreter.attach(lPagePtr);
            byte* lTuplePtr = lInterpreter.getRecord(tid.tupleNo());
            if(lTuplePtr)
            { 
                t.toMemory(lTuplePtr);
                result.push_back(t); 
            }
            else
            {
                throw TupleNotFoundOrInvalidException(FLF);
            }

            if(curr != prev && prev != INVALID)
            {
                TRACE("released prev page");
                releasePage(prev);
            }
        }
        else
        {
            throw TupleNotFoundOrInvalidException(FLF);
        }
    }
    if(curr != INVALID)
    {
        releasePage(curr);
    }
    return result;
}

/*
template<typename Tuple_T>
tid_vt SegmentFSM_SP::insertTuples(const std::vector<Tuple_T>& aTupleVector)
{
    TRACE("trying to insert many Tuples");
	// get page with enough space for the tuples and load it into memory
	tid_vt result;
    bool emptyfix = false;
    size_t lTotalSize = 0;
    for (auto& iter : aTupleVector)
    {
        // slotted page is 8byte aligned, allign tuple size
        lTotalSize += std::ceil(((double) iter.size()) / 8.0) * 8;
        lTotalSize += sizeof(InterpreterSP::slot_t);
    }
    PID lPID;
    try
    {
	    lPID = SegmentFSM::getFreePage(lTotalSize, emptyfix, sizeof(sp_header_t));
    }
    // exception handling buggy, will be fixed tomorrow
    catch(NSMException ex)
    {
        // lTotalSize is bigger than size of a page. 
        // Now we try to approx the bin packing problem (NP hard) by a greedy algorithm...
        uint lPageSize = std::ceil(((getPageSize() - sizeof(sp_header_t)) / 16) * 15);
        uint lPartSize;
        uint start = 0;
        uint end = 0;
        while(start < aTupleVector.size())
        {
            lPartSize = 0;
            start = end;
            while ((lPartSize < lPageSize) & (end < aTupleVector.size()))
            {
                // slotted page is 8byte aligned, allign tuple size
                lPartSize += std::ceil(((double) aTupleVector.at(end).size() ) / 8.0) * 8;
                lPartSize += sizeof(InterpreterSP::slot_t);
                ++end;
            }
            if(lPartSize > lPageSize)
            {
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
    // fix page, if it is new, use different command on buffer
	if(emptyfix)
    {
		lBCB = _bufMan.emptyfix(lPID);
	}
	else
    {
		lBCB = _bufMan.fix(lPID,  LOCK_MODE::kEXCLUSIVE); 
	}
    // get iterator to PID in page vector
    auto it = std::find_if(_pages.begin(), _pages.end(), [lPID] (const std::pair<PID, BCB*>& elem)
    { 
        return elem.first == lPID;
    });
    if(it != _pages.end())
    {
        TRACE("## Page found! Assign BCB Pointer to _pages vector");
        it->second = lBCB;
    }
    else
    {
        TRACE("## This should not be printed");
        // terminate and find bug
        throw ReturnException(FLF);
    }
	byte* lBufferPage = _bufMan.getFramePtr(lBCB);

	InterpreterSP lInterpreter;
	// if the page is new, it has to be initialised first.
	if(emptyfix)
    {
		lInterpreter.initNewPage(lBufferPage);
	}

	// attach page to sp interpreter
	lInterpreter.attach(lBufferPage);
	for(auto& lTuple : aTupleVector)
	{
        // if enough space is free on nsm page, the pointer will point to location on page where to insert a tuple
        auto [tplPtr, tplNo] = lInterpreter.addNewRecord(lTuple.size()); // C++17 Syntax. Return is a pair, assign pair.first to tplPtr and pair.second to tplNo
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
} */
