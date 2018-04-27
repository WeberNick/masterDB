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

class BufferManager
{
    private:
         //list of free frame indexes. protected by a mutex
        class FreeFrames
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
                inline size_t*  getFreeFrameList(){ return _freeFrameList; }
                inline sMtx&    getFreeFrameListMtx(){ return _freeFrameListMtx; }
                inline size_t   getNoFreeFrames(){ return _noFreeFrames; }
                inline size_t   incrNoFreeFrames(){ return ++_noFreeFrames; }
                inline size_t   decrNoFreeFrames(){ return --_noFreeFrames; }
                inline void     setNoFreeFrames(size_t aNoFreeFrames){ _noFreeFrames = aNoFreeFrames; }

            private:
                size_t* _freeFrameList;
                sMtx    _freeFrameListMtx;
		        size_t 	_noFreeFrames;

        };
       //linked list of free control blocks. protected by a mutex
        class FreeBCBs
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
                inline BCB*     getFreeBCBList(){ return _freeBCBList; }
                inline sMtx&    getFreeBCBListMtx(){ return _freeBCBListMtx; }
                inline size_t   getNoFreeBCBs(){ return _noFreeBCBs; }
                inline size_t   incrNoFreeBCBs(){ return ++_noFreeBCBs; }
                inline size_t   decrNoFreeBCBs(){ return --_noFreeBCBs; }
                inline void     setFreeBCBList(BCB* aFreeBCB){ _freeBCBList = aFreeBCB; }
                inline void     setNoFreeBCBs(size_t aNoFreeBCBs){ _noFreeBCBs = aNoFreeBCBs; }

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
        inline static BufferManager& getInstance()
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
        inline size_t   getNoFrames(){ return _noFrames; }
        inline size_t   getFrameSize(){ return _frameSize; }
        
    private:
        BCB*                locatePage(const PID& aPageID);
        void                readPageIn(BCB* lFBCB, const PID& aPageID);
        void                initNewPage(BCB* aFBCB, const PID& aPageID, uint64_t aLSN);
        size_t              getFrame();

    private:
        inline FreeFrames&  getFreeFrames(){ return _freeFrames; }
        inline FreeBCBs&    getFreeBCBs(){ return _freeBCBs; }

	private:
		size_t              _noFrames;
		size_t 		        _frameSize;;
		BufferHashTable*    _bufferHash;
		byte* 			    _bufferpool;
        FreeFrames          _freeFrames;
        FreeBCBs            _freeBCBs;
        const CB*           _cb;
        bool                _init; 

};
