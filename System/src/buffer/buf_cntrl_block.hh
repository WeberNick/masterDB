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
#include "infra/exception.hh"
#include "infra/trace.hh"

#include <mutex>
#include <shared_mutex>

class BufferControlBlock;
using BCB = BufferControlBlock;

class BufferControlBlock
{
    private:
        friend class BufferManager;
        friend class BufferHashTable;
        explicit BufferControlBlock();
        BufferControlBlock(const BufferControlBlock&) = delete;
        BufferControlBlock(BufferControlBlock&&) = delete;
        BufferControlBlock& operator=(const BufferControlBlock&) = delete;
        BufferControlBlock& operator=(BufferControlBlock&&) = delete;
        ~BufferControlBlock();


    public:
        //getter
        inline PID&     getPID(){ return _pageID; }
        inline size_t   getFrameIndex(){ return _frameIndex; }
        inline sMtx&    getMtx(){ return _pageMtx; }
        inline LOCK_MODE getLockMode(){ return _mode; }
        inline bool     getModified(){ return _modified; }
        inline size_t   getFixCount(){ return _fixCount; }
        inline size_t   incrFixCount(){ return ++_fixCount; }
        inline size_t   decrFixCount(){ return --_fixCount; }
        inline BCB*     getNextInChain(){ return _nextInChain; }
        //setter
        inline void     setPID(const PID aPID){ _pageID = aPID; }
        inline void     setFrameIndex(const size_t aFrameIndex){ _frameIndex = aFrameIndex; }
        inline void     setLockMode(LOCK_MODE aMode){ _mode = aMode; }
        inline void     setModified(const bool aModified){ _modified = aModified; }
        inline void     setFixCount(const int aFixCount){ _fixCount = aFixCount; }
        inline void     setNextInChain(BCB* aBCB){ _nextInChain = aBCB; }

    public:
        void upgradeLock(LOCK_MODE aMode);

    private:
        static const CB*  _cb;
        static void setCB(const CB* aControlBlock);

    private:
        PID         _pageID; 
        size_t      _frameIndex;    //buffer pool index where page now stored
        sMtx        _pageMtx;       //semaphore to protect page
        LOCK_MODE   _mode;
        bool        _modified;
        size_t      _fixCount;
        //LSN infos
        BCB*        _nextInChain;   //hash overflow chain forward pointer
};
