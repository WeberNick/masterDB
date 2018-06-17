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
         //list of free frame indexes. protected by a mutex
        class FreeFrames final
        {
            public:
                explicit FreeFrames() : _freeFrameList(nullptr), _freeFrameListMtx(), _noFreeFrames(0){}
                explicit FreeFrames(const FreeFrames&) = delete;
                explicit FreeFrames(FreeFrames&&) = delete;
                FreeFrames& operator=(const FreeFrames&) = delete;
                FreeFrames& operator=(FreeFrames&&) = delete;
                ~FreeFrames(){ delete[] _freeFrameList; }

            public:
                void init(const size_t aNoFreeFrames);
                
            public:
                inline size_t*  getFreeFrameList() noexcept { return _freeFrameList; }
                inline sMtx&    getFreeFrameListMtx() noexcept { return _freeFrameListMtx; }
                inline size_t   getNoFreeFrames() noexcept { return _noFreeFrames; }
                inline size_t   incrNoFreeFrames() noexcept { return ++_noFreeFrames; }
                inline size_t   decrNoFreeFrames() noexcept { return --_noFreeFrames; }
                inline void     setNoFreeFrames(size_t aNoFreeFrames) noexcept { _noFreeFrames = aNoFreeFrames; }

            private:
                size_t* _freeFrameList;
                sMtx    _freeFrameListMtx;
		        size_t 	_noFreeFrames;

        };
       //linked list of free control blocks. protected by a mutex
        class FreeBCBs final
        {
            public:
                explicit FreeBCBs() : _BCBs(), _freeBCBList(nullptr), _freeBCBListMtx(), _noFreeBCBs(0){}
                explicit FreeBCBs(const FreeBCBs&) = delete;
                explicit FreeBCBs(FreeBCBs&&) = delete;
                FreeBCBs& operator=(const FreeBCBs&) = delete;
                FreeBCBs& operator=(FreeBCBs&&) = delete;
                ~FreeBCBs()
                {
                    for(BCB* ptr : _BCBs)
                    {
                        delete ptr; 
                    }
                }

            public:
                void init(const size_t aNoFreeBCBs);

            public:
                inline BCB*     getFreeBCBList() noexcept { return _freeBCBList; }
                inline sMtx&    getFreeBCBListMtx() noexcept { return _freeBCBListMtx; }
                inline size_t   getNoFreeBCBs() noexcept { return _noFreeBCBs; }
                inline size_t   incrNoFreeBCBs() noexcept { return ++_noFreeBCBs; }
                inline size_t   decrNoFreeBCBs() noexcept { return --_noFreeBCBs; }
                inline void     setFreeBCBList(BCB* aFreeBCB) noexcept { _freeBCBList = aFreeBCB; }
                inline void     setNoFreeBCBs(size_t aNoFreeBCBs) noexcept { _noFreeBCBs = aNoFreeBCBs; }
                inline void     freeBCB(BCB* aBCB) noexcept { aBCB->setNextInChain(getFreeBCBList()); setFreeBCBList(aBCB); };
                inline std::vector<BCB*> getAllBCBs() noexcept { return _BCBs; }
                void            resetBCB(BCB* aBCB) noexcept; //used to reset a BCB after the page it corresponds to was deleted

            private:
                //containing all BCB pointer. With this vector it is convenient to free the memory later
                std::vector<BCB*> _BCBs;
                //pointer to first element in the list of free buffer control blocks
                BCB*    _freeBCBList;
                //Mutex protecting the list of free buffer control blocks
                sMtx    _freeBCBListMtx;
                //number of free buffer control blocks
                size_t  _noFreeBCBs;
        };

    private:
        explicit BufferManager();
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

        void init(const CB& aControlBlock);

    public:
        /* request access to a page and fix it */
        BCB* fix(const PID& aPageID, LOCK_MODE aMode);
        BCB* emptyfix(const PID& aPageID);
        /* unfix a page */
        void unfix(BCB*& aBufferControlBlock);
        /* write page to disk */
        void  flush(BCB*& aBufferControlBlock);
        void flushAll();

    public:
        byte* getFramePtr(BCB* aBCB);

    public:
        inline size_t   getNoFrames() noexcept { return _noFrames; }
        inline size_t   getFrameSize() noexcept { return _frameSize; }
        inline void     resetBCB(BCB* aBCB) noexcept { _freeBCBs.resetBCB(aBCB); }
        void            resetBCB(PID& aPID) noexcept;
        
    private:
        BCB*                locatePage(const PID& aPageID) noexcept;
        void                readPageIn(BCB* lFBCB, const PID& aPageID);
        void                initNewPage(BCB* aFBCB, const PID& aPageID, uint64_t aLSN);
        size_t              getFrame() noexcept;

    private:
        inline FreeFrames&  getFreeFrames() noexcept { return _freeFrames; }
        inline FreeBCBs&    getFreeBCBs() noexcept { return _freeBCBs; }

	private:
		size_t              _noFrames;
		size_t 		        _frameSize;;
		BufferHashTable*    _bufferHash;
		byte* 			    _bufferpool;
        FreeFrames          _freeFrames;
        FreeBCBs            _freeBCBs;
        const CB*           _cb;
};
