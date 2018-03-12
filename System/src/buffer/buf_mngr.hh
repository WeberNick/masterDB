/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the buffer manager	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#ifndef BUFFER_MANAGER_HH
#define BUFFER_MANAGER_HH

#pragma once

#include "infra/types.hh"
#include "buf_hash_table.hh"
#include "buf_cntrl_block.hh"

#include <cstddef>
#include <mutex>
#include <shared_mutex>

class BufferManager
{
    public:
		explicit BufferManager(const size_t aNoFrames, const control_block_t& aControlBlock);
        BufferManager(const BufferManager& aBufferManager) = delete;
        BufferManager &operator=(const BufferManager& aBufferManager) = delete;
        ~BufferManager();

    public:
        /* request access to a page and fix it */
        bool fix(const pid aPageID, const LOCK_MODE aMode, byte*& aPagePointer);
        bool emptyfix(const pid aPageID, const LOCK_MODE aMode, byte*& aPagePointer);
        /* unfix a page */
        bool unfix(byte* aPagePointer);
        /* write page to disk */
        bool flush(byte* aPagePointer);

    public:
        BCB*    getBufCb();
        void    setBufCb(BCB* aBufCb);

    private:
        BCB*    locatePage(const pid aPageID, const size_t aHashIndex);
        int     getFrame();


	private:
		size_t      		_noFrames;
		size_t 		        _frameSize;;
		BufferHashTable* 	_bufferHash;
		byte** 				_bufferpool;
		uint*				_freeFrameIndexes;
        sMtx                _freeFrameMutex;
		unsigned long 		_noFreeFrames;

        const control_block_t&    _controlBlock;
};

#endif
