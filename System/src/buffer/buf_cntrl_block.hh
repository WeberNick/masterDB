/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Class implementing a access control block for the buffer franes	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#ifndef BUFFER_CONTROL_BLOCK_HH
#define BUFFER_CONTROL_BLOCK_HH
#pragma once

#include "infra/types.hh"

#include <mutex>
#include <shared_mutex>

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
        inline pid&     getPID(){ return _pageID; }
        inline uint     getFrameIndex(){ return _frameIndex; }
        inline sMtx&    getMutex(){ return _pageMutex; }
        inline int      getFixCount(){ return _fixCount; }
        inline void     setFixCount(const int aFixCount){ _fixCount = aFixCount; }
        inline void     incrFixCount(){ ++_fixCount; }
        inline void     decrFixCount(){ --_fixCount; }


        //...


        inline BCB*     getNextInChain(){ return _nextInChain; }

    private:
        pid     _pageID;    //
        //page handle?
        uint    _frameIndex;    //buffer pool index where page now stored
        sMtx    _pageMutex;       //semaphore to protect page
        bool    _modified;
        int     _fixCount;
        //LSN infos
        BCB*    _nextInChain;   //hash overflow chain forward pointer
};
#endif
