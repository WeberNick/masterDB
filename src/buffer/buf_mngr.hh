/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *          Jonas Thietke
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

/**
 * @brief Singleton class implementing 
 */
class BufferManager final
{
    private:
         //list of free frame indexes. protected by a mutex
         //changed into a stack
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
                inline sMtx&    getFreeFrameListMtx() noexcept { return _freeFrameListMtx; }
                inline size_t   getNoFreeFrames() noexcept { return _noFreeFrames; }
                inline size_t   incrNoFreeFrames() noexcept { return ++_noFreeFrames; }
                inline size_t   decrNoFreeFrames() noexcept { return --_noFreeFrames; }
                inline void     setNoFreeFrames(size_t aNoFreeFrames) noexcept { _noFreeFrames = aNoFreeFrames; }

            private:
                std::unique_ptr<size_t[]> _freeFrameList;
                sMtx    _freeFrameListMtx;
		        size_t 	_noFreeFrames;

        };
       //linked list of free control blocks. protected by a mutex, also only accessible as a LIFO queue
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
                void            resetBCB(BCB* aBCB) noexcept; //used to reset a BCB after the page it corresponds to was deleted

            private:
                inline size_t   incrNoFreeBCBs() noexcept { return ++_noFreeBCBs; }
                inline size_t   decrNoFreeBCBs() noexcept { return --_noFreeBCBs; }

            private:
                //containing all BCB pointer. With this vector it is convenient to free the memory later
                std::vector<std::unique_ptr<BCB>> _BCBs;
                //pointer to first element in the list of free buffer control blocks
                BCB*    _freeBCBList;
                //Mutex protecting the list of free buffer control blocks
                sMtx    _freeBCBListMtx;
                //number of free buffer control blocks
                size_t  _noFreeBCBs;
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

        void init(const CB& aControlBlock) noexcept;

    public:
        /**
         * @brief   puts an existing page into the buffer, the main functionallity of the buffer
         * @param   aPageID     ID of page to be loaded
         * @param   aMode       LOCK_MODE the requested page shall have in the end
         * @return  a BCB containing the requested page in the requested LOCK_MODE
        /* request access to a page and fix it */
        BCB* fix(const PID& aPageID, LOCK_MODE aMode);
        /**
         * @brief   if the page does not exist yet, is does not have to be searched or loaded.
         *          Assumes the page not to loaded before, will produce unexpected behaviour if missused!
         * @param   aPageID     page to be loaded and initialised
         * @return  a BCB containing the requested page exclusively locked
         */
        BCB* emptyfix(const PID& aPageID);
        /**
         * @brief   releases the page again so that it can be displaced if necessary
         * @param   aBufferControlBlock     BCB which shall be released
         */
        void unfix(BCB*& aBufferControlBlock);
        /** 
         * @brief   writes page to disk. Does not exclude it from the buffer
         * @param   aBufferControlBlock     BCB which shall be flushed
         */
        void  flush(BCB*& aBufferControlBlock);
        /**
         * @brief   flushed the complete buffer. used for shutdown or to save a current state of the system.
         */
        void flushAll();

    public:
        byte* getFramePtr(BCB* aBCB);

    public:
        inline size_t   getNoFrames() noexcept { return _noFrames; }
        inline size_t   getFrameSize() noexcept { return _frameSize; }
        /**
         * @brief   if a BCB gets out of use, it has to be excluded from the buffer and inserted into the freeBCB List
         * @param   aPID    PID of the page to be excluded from the buffer, the BCB is searched.
         */
        void            resetBCB(const PID& aPID) noexcept;
        
    private:
        /**
         * @brief   prepares a BCB so that the page can be inserted into the buffer
         * @param   aPageID     PID the BCB shall contain afterwards
         * @return  the BCB the page shall be loaded into
         */
        BCB*                locatePage(const PID& aPageID) noexcept;
        /**
         * @brief   reads Page from Disk into a Buffer Frame
         * @param   lFBCB       free BCB which shall contain the page afterwards
         * @param   aPageID     the page that shall be read in
         */
        void                readPageIn(BCB* lFBCB, const PID& aPageID);
        /**
         * @brief   if emptyfix is called, the page does not contain a header. 
         *          This method creates it and removes all data which was previously stored on the frame.
         * @param   aFBCB       BCB which contains the page
         * @param   aPageID     the page that shall be contained by this BCB afterwards
         * @param   aLSN        LSN of this page, needed to initialise a header
         */
        void                initNewPage(BCB* aFBCB, const PID& aPageID, uint64_t aLSN);
        /**
         * @brief   every BCB contains a Frame to actually contain the page and there amount is limited.
         *          if the _freeFrames list contains frames, it just returns on
         *          if this is not the case, it chooses a BCB from the buffer by random to get evicted.
         */
        size_t              getFrame() noexcept;

    private:
        inline FreeFrames&  getFreeFrames() noexcept { return _freeFrames; }
        inline FreeBCBs&    getFreeBCBs() noexcept { return _freeBCBs; }

	private:
		size_t              _noFrames;
		size_t 		        _frameSize;;
        std::unique_ptr<BufferHashTable>    _bufferHash;
        std::unique_ptr<byte[]>             _bufferpool;
        FreeFrames          _freeFrames;
        FreeBCBs            _freeBCBs;
        const CB*           _cb;
};


BCB* BufferManager::FreeBCBs::popFromList()
{
    if(_freeBCBList)
    {
        lock();
        BCB* result = _freeBCBList;
        _freeBCBList = result->getNextInChain();
        result->setNextInChain(nullptr);
        decrNoFreeBCBs(); //decrement number of free BCBs
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
