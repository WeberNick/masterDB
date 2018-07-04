/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Class implementing a access control block for the buffer franes	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"

#include <mutex>
#include <shared_mutex>
#include <string>

class BufferControlBlock;
using BCB = BufferControlBlock;

class BufferControlBlock final
{
    public:
        BufferControlBlock();
        explicit BufferControlBlock(const BufferControlBlock&) = delete;
        explicit BufferControlBlock(BufferControlBlock&&) = delete;
        BufferControlBlock& operator=(const BufferControlBlock&) = delete;
        BufferControlBlock& operator=(BufferControlBlock&&) = delete;
        ~BufferControlBlock() = default;

    public:
        // Getter
        inline PID&     getPID() noexcept { return _pageID; }
        inline size_t   getFrameIndex() noexcept { return _frameIndex; }
        inline LOCK_MODE getLockMode() noexcept { return _mode; }
        inline bool     getModified() noexcept { return _modified; }
        inline size_t   getFixCount() noexcept { return _fixCount; }
        inline size_t   incrFixCount() noexcept { return ++_fixCount; }
        inline size_t   decrFixCount() noexcept { return --_fixCount; }
        inline BCB*     getNextInChain() noexcept { return _nextInChain; }
        // Setter
        inline void     setPID(const PID aPID) noexcept { _pageID = aPID; }
        inline void     setFrameIndex(const size_t aFrameIndex) noexcept { _frameIndex = aFrameIndex; }
        inline void     setLockMode(LOCK_MODE aMode) noexcept { _mode = aMode; }
        inline void     setModified(const bool aModified) noexcept { _modified = aModified; }
        inline void     setFixCount(const int aFixCount) noexcept { _fixCount = aFixCount; }
        inline void     setNextInChain(BCB* aBCB) noexcept { _nextInChain = aBCB; }

    public:
        void lock(LOCK_MODE aMode) noexcept; // lock for given mode
        inline void lock() noexcept;         // lock exclusive
        inline bool try_lock() noexcept;
        inline void lock_shared() noexcept;  // lock shared
        void unlock() noexcept;
        void upgradeLock(LOCK_MODE aMode) noexcept;
        inline std::string to_string() noexcept;

    private:
        PID         _pageID; 
        size_t      _frameIndex;    // buffer pool index where page now stored
        sMtx        _pageMtx;       // semaphore to protect page
        LOCK_MODE   _mode;
        bool        _modified;
        size_t      _fixCount;
        //LSN infos
        BCB*        _nextInChain;   // hash overflow chain forward pointer
};

void BufferControlBlock::lock() noexcept
{
    lock(LOCK_MODE::kEXCLUSIVE);
}

bool BufferControlBlock::try_lock() noexcept
{
    const bool ret = _pageMtx.try_lock();
    if(ret)
    {
        setLockMode(LOCK_MODE::kEXCLUSIVE);
        setFixCount(1);
    }
    return ret;
}

void BufferControlBlock::lock_shared() noexcept
{
    lock(LOCK_MODE::kSHARED);
}

std::string BufferControlBlock::to_string() noexcept 
{ 
    return std::string("PID : '") + getPID().to_string() 
        + "', Frame Index : " + std::to_string(getFrameIndex()) 
        + ", Lock Mode : '" + lockModeToString(getLockMode()) 
        + ", Modified : '" + (getModified() ? "True" : "False") 
        + "', Fix Count : " + std::to_string(getFixCount()); 
}

std::ostream& operator<< (std::ostream& stream, BCB* aBCB);
