/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the buffer manager	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#pragma once

#include "infra/types.hh"
#include "buf_hash_table.hh"
#include "buf_cntrl_block.hh"

#include "partition/partition_manager.hh"
#include "partition/partition_base.hh"

#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <random>

class BufferManager
{
    private:
         //list of free frame indexes. protected by a mutex
        class FreeFrames
        {
            public:
                explicit FreeFrames(const size_t aNoFreeFrames) :
                    _freeFrameList(new size_t[aNoFreeFrames]), _freeFrameListMtx(), _noFreeFrames(aNoFreeFrames)
                {
                    for(size_t i = 0; i < _noFreeFrames; ++i)
                    {
                        _freeFrameList[i] = i;
                    }
                }
                FreeFrames(const FreeFrames&) = delete;
                FreeFrames(FreeFrames&&) = delete;
                FreeFrames& operator=(const FreeFrames&) = delete;
                FreeFrames& operator=(FreeFrames&&) = delete;
                ~FreeFrames()
                { delete[] _freeFrameList; }

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
                explicit FreeBCBs(const size_t aNoFreeBCBs) :
                    _freeBCBList(nullptr), _freeBCBListMtx(), _noFreeBCBs(aNoFreeBCBs)
                {
                    BCB* newBCB = new BCB;
                    _freeBCBList = newBCB;
                    size_t i = 0;
                    const size_t lNoFreeBCB = _noFreeBCBs - 1; //one is already created
                    //create the initial list of free BCBs
                    while(i < lNoFreeBCB)
                    {
                        newBCB->_nextInChain = new BCB;
                        newBCB = newBCB->_nextInChain;
                        ++i;
                    }
                }
                FreeBCBs(const FreeBCBs&) = delete;
                FreeBCBs(FreeBCBs&&) = delete;
                FreeBCBs& operator=(const FreeBCBs&) = delete;
                FreeBCBs& operator=(FreeBCBs&&) = delete;
                ~FreeBCBs()
                { /*TODO*/}

            public:
                inline BCB*     getFreeBCBList(){ return _freeBCBList; }
                inline sMtx&    getFreeBCBListMtx(){ return _freeBCBListMtx; }
                inline size_t   getNoFreeBCBs(){ return _noFreeBCBs; }
                inline size_t   incrNoFreeBCBs(){ return ++_noFreeBCBs; }
                inline size_t   decrNoFreeBCBs(){ return --_noFreeBCBs; }
                inline void     setFreeBCBList(BCB* aFreeBCB){ _freeBCBList = aFreeBCB; }
                inline void     setNoFreeBCBs(size_t aNoFreeBCBs){ _noFreeBCBs = aNoFreeBCBs; }

            private:
                //pointer to first element in the list of free buffer control blocks
                BCB*    _freeBCBList;
                //Mutex protecting the list of free buffer control blocks
                sMtx    _freeBCBListMtx;
                //number of free buffer control blocks
                size_t  _noFreeBCBs;
        };

    public:
        explicit BufferManager(const size_t aNoFrames, const control_block_t& aControlBlock);
        BufferManager(const BufferManager&) = delete;
        BufferManager(BufferManager&&) = delete;
        BufferManager& operator=(const BufferManager&) = delete;
        BufferManager& operator=(BufferManager&&) = delete;
        ~BufferManager();

    public:
        static BufferManager& getInstance(){
            static BufferManager lBufferManagerInstance = BufferManager(10,control_block_t {"a",10,0});
            return lBufferManagerInstance;
        }
        /* request access to a page and fix it */
        BCB* fix(const pid aPageID);
        BCB* emptyfix(const pid aPageID);
        /* unfix a page */
        void unfix(BCB*& aBufferControlBlock);
        /* write page to disk */
        void  flush(BCB*& aBufferControlBlock);
        void flushAll();

    public:
        inline size_t   getNoFrames(){ return _noFrames; }
        inline size_t   getFrameSize(){ return _frameSize; }
        inline byte*    getFramePtr(BCB* aBCB)
        {
            const size_t lFrameIndex = aBCB->getFrameIndex();   
            return (lFrameIndex < _noFrames) ? (_bufferpool + (lFrameIndex * _frameSize)) : nullptr;
        }

    private:
        BCB*                locatePage(const pid aPageID, const size_t aHashIndex);
        size_t              getFrame();

    private:
        inline FreeFrames&  getFreeFrames(){ return _freeFrames; }
        inline FreeBCBs&    getFreeBCBs(){ return _freeBCBs; }

	private:
		size_t      		    _noFrames;
		size_t 		            _frameSize;;
		BufferHashTable* 	    _bufferHash;
		byte* 				    _bufferpool;
        FreeFrames              _freeFrames;
        FreeBCBs                _freeBCBs;
        const control_block_t&  _controlBlock;

};
