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
#include "infra/semaphore.hh"
#include "buf_hash_table.hh"
#include "buf_acc_cntrl_block.hh"
#include "buf_cntrl_block.hh"

#include <cstddef>

class BufferManager
{
    public:
		explicit BufferManager(const size_t aNoFrames, const size_t aFrameSize = 4096);
        BufferManager(const BufferManager& aBufferManager) = delete;
        BufferManager &operator=(const BufferManager& aBufferManager) = delete;
        ~BufferManager();

    public:
        /* request access to a page and fix it */
        bool fix(const pid aPageID, const LOCK_MODE aMode, BACB* const  aBufferAccCbPointer);
        bool emptyfix(const pid aPageID, const LOCK_MODE aMode, BACB* const  aBufferAccCbPointer);
        /* unfix a page */
        bool unfix(BACB* const aBufferAccCbPointer);
        /* write page to disk */
        bool flush(const BACB aBufferAccCb);


	private:
		size_t      		_noFrames;
		size_t 		        _frameSize;;
		BufferHashTable* 	_bufferHash;
		byte** 				_bufferpool;
		uint*				_freeFrameIndexes;
		Semaphore 			_freeFrameSem;
		unsigned long 		_noFreeFrames;

		//pointer to control block of most recently used page
        BCB*        		_pageMRU;   
        //pointer to control block of last recently used page
        BCB*        		_pageLRU;           
        //Semaphore protecting lru  list
        Semaphore           _LRUSem;    

 	 	//pointer to the first element in the list of free buffer access control blocks
        BACB*    			_freeAccCbList;
        //semaphore to protect operations on the list of free access control blocks
        Semaphore           _freeAccCbSem;
        //current number of free access control blocks
        uint                _noFreeAccCb;


};
