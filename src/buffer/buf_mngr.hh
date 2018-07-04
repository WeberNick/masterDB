/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the buffer manager	
 *  @bugs	Currently no bugs known
 *  @todos  -Exception Handling in readPageIn,
 *          -Should initNewPage be in buf mngr or somewhere else?
 *  @section TBD
 */
#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../partition/partition_manager.hh"
#include "../partition/partition_base.hh"

#include "buf_hash_table.hh"
#include "buf_cntrl_block.hh"

#include <vector>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <new>
#include <chrono>
#include <thread>

class BufferManager final
{
    private:
        // list of free frame indexes. protected by a mutex
        // changed into a stack
        class FreeFrames final
        {
            public:
                FreeFrames();
                explicit FreeFrames(const FreeFrames&) = delete;
                explicit FreeFrames(FreeFrames&&) = delete;
                FreeFrames& operator=(const FreeFrames&) = delete;
                FreeFrames& operator=(FreeFrames&&) = delete;
                ~FreeFrames() = default;

            public:
                void init(const size_t aNoFreeFrames) noexcept;
                size_t pop();
                void push(size_t aFrameNo);
                
            private:
                inline const auto& getFreeFrameList() noexcept { return _freeFrameList; }
                inline sMtx&       getFreeFrameListMtx() noexcept { return _freeFrameListMtx; }
                inline size_t      getNoFreeFrames() noexcept { return _noFreeFrames; }
                inline size_t      incrNoFreeFrames() noexcept { return ++_noFreeFrames; }
                inline size_t      decrNoFreeFrames() noexcept { return --_noFreeFrames; }
                inline void        setNoFreeFrames(size_t aNoFreeFrames) noexcept { _noFreeFrames = aNoFreeFrames; }

            private:
                std::unique_ptr<size_t[]> _freeFrameList;
                sMtx    _freeFrameListMtx;
		        size_t 	_noFreeFrames;

        };

        // linked list of free control blocks. protected by a mutex
        class FreeBCBs final
        {
            public:
                FreeBCBs();
                explicit FreeBCBs(const FreeBCBs&) = delete;
                explicit FreeBCBs(FreeBCBs&&) = delete;
                FreeBCBs& operator=(const FreeBCBs&) = delete;
                FreeBCBs& operator=(FreeBCBs&&) = delete;
                ~FreeBCBs() = default;

            public:
                void init(const size_t aNoFreeBCBs) noexcept;

            public:
                inline BCB*     getFreeBCBList() noexcept { TRACE("first BCB in line is "+_freeBCBList->to_string());  return _freeBCBList; }
                inline void     lock() noexcept { _freeBCBListMtx.lock(); }
                inline void     unlock() noexcept { _freeBCBListMtx.unlock(); }
                inline size_t   getNoFreeBCBs() noexcept { return _noFreeBCBs; }
                inline BCB*     popFromList();
                inline void     insertToFreeBCBs(BCB* aBCB) noexcept;
                // inline void  setNoFreeBCBs(size_t aNoFreeBCBs) noexcept { _noFreeBCBs = aNoFreeBCBs; }
                void            resetBCB(BCB* aBCB) noexcept; //used to reset a BCB after the page it corresponds to was deleted

            private:
                inline size_t   incrNoFreeBCBs() noexcept { return ++_noFreeBCBs; }
                inline size_t   decrNoFreeBCBs() noexcept { return --_noFreeBCBs; }

            private:  
                std::vector<std::unique_ptr<BCB>> _BCBs; // containing all BCB pointer. With this vector it is convenient to free the memory later 
                BCB*    _freeBCBList;                    // pointer to first element in the list of free buffer control blocks 
                sMtx    _freeBCBListMtx;                 // Mutex protecting the list of free buffer control blocks 
                size_t  _noFreeBCBs;                     // number of free buffer control blocks
        };

    private:
        BufferManager();
        explicit BufferManager(const BufferManager&) = delete;
        explicit BufferManager(BufferManager&&) = delete;
        BufferManager& operator=(const BufferManager&) = delete;
        BufferManager& operator=(BufferManager&&) = delete;
        ~BufferManager();

    public:
        inline static BufferManager& getInstance() noexcept
        {
            static BufferManager lBufferManagerInstance; 
            return lBufferManagerInstance;
        }

        /** TODO
         * @brief 
         * 
         * @param aControlBlock 
         */
        void init(const CB& aControlBlock) noexcept;

    public:
        /**
         * @brief request access to a page and fix it
         * 
         * @param aPageID the page id
         * @param aMode the lock mode
         * @return BCB* a pointer to the buffer control block
         */
        BCB* fix(const PID& aPageID, LOCK_MODE aMode);
        BCB* emptyfix(const PID& aPageID);
        /**
         * @brief unfix a page
         * 
         * @param aBufferControlBlock the buffer control block
         */
        void unfix(BCB*& aBufferControlBlock);
        /**
         * @brief Write page to disk
         * 
         * @param aBufferControlBlock the buffer control block
         */
        void flush(BCB*& aBufferControlBlock);
        void flushAll();

    public:
        byte* getFramePtr(BCB* aBCB);

    public:
        inline size_t getNoFrames() noexcept { return _noFrames; }
        inline size_t getFrameSize() noexcept { return _frameSize; }
        void          resetBCB(const PID& aPID) noexcept;
        
    private:
        BCB*   locatePage(const PID& aPageID) noexcept;
        void   readPageIn(BCB* lFBCB, const PID& aPageID);
        void   initNewPage(BCB* aFBCB, const PID& aPageID, uint64_t aLSN);
        size_t getFrame() noexcept;

    private:
        inline FreeFrames& getFreeFrames() noexcept { return _freeFrames; }
        inline FreeBCBs&   getFreeBCBs() noexcept { return _freeBCBs; }

	private:
		size_t                              _noFrames;
		size_t 		                        _frameSize;;
        std::unique_ptr<BufferHashTable>    _bufferHash;
        std::unique_ptr<byte[]>             _bufferpool;
        FreeFrames                          _freeFrames;
        FreeBCBs                            _freeBCBs;
        const CB*                           _cb;
};


BCB* BufferManager::FreeBCBs::popFromList()
{
    if(_freeBCBList)
    {
        lock();
        BCB* result = _freeBCBList;
        _freeBCBList = result->getNextInChain();
        result->setNextInChain(nullptr);
        decrNoFreeBCBs(); // decrement number of free BCBs
        unlock();
        return result;
    }
    throw NoFreeBCBsException(FLF);
}

void BufferManager::FreeBCBs::insertToFreeBCBs(BCB* aBCB) noexcept 
{ 
    lock();
    aBCB->setNextInChain(_freeBCBList);
    _freeBCBList = aBCB; 
    incrNoFreeBCBs();
    unlock();
}
