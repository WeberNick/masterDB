/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Class implementing a access control block for the buffer franes	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#pragma once

#include "infra/types.hh"
#include "infra/semaphore.hh"

class BufferControlBlock;
typedef BufferControlBlock BCB;

class BufferControlBlock
{
    private:
        friend class BufferManager;
        friend class BufferHashTable;
        explicit BufferControlBlock();
		BufferControlBlock(const BufferControlBlock&) = delete;
        BufferControlBlock &operator=(const BufferControlBlock&) = delete;
        ~BufferControlBlock();


    public:
        inline pid      getPID(){ return _pageID; }
        inline uint     getFrameIndex(){ return _frameIndex; }

        inline int      getFixCount(){ return _fixCount; }
        inline void     setFixCount(const int aFixCount){ _fixCount = aFixCount; }
        inline void     incrFixCount(){ ++_fixCount; }
        inline void     decrFixCount(){ --_fixCount; }


        //...


        inline BCB*     getNextInChain(){ return _nextInChain; }

    private:
        pid         _pageID;    //
        //page handle?
        uint            _frameIndex;    //buffer pool index where page now stored
        Semaphore       _pageSem;       //semaphore to protect page
        bool            _modified;
        int             _fixCount;
        //LSN infos
        BufferControlBlock*    _prevInLRU;     //prev. page in LRU chain
        BufferControlBlock*    _nextInLRU;     //next page in LRU chain
        BufferControlBlock*    _nextInChain;   //hash overflow chain forward pointer
};
