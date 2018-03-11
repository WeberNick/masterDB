/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the hash table of the buffer manager
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#pragma once

#include "infra/types.hh"
#include "infra/semaphore.hh"
#include "buf_cntrl_block.hh"

#include <cstddef>
#include <functional>

class BufferHashTable
{
    public:
        struct bucket_t
        {
            //each bucket is protected by a semaphore while being used
            Semaphore   _bucketSem;     
            //pointer to first control block
            BCB*        _firstBufCb;    
        };


	public:
		explicit BufferHashTable(const std::size_t aHashTableSize);
		BufferHashTable(const BufferHashTable& aBufferHashTable) = delete;
        BufferHashTable &operator=(const BufferHashTable& aBufferHashTable) = delete;
        ~BufferHashTable();


	private:
		std::size_t hash(const pid aPageID);


	private:
        //the size of the hash table
		std::size_t _size;      		
        //the hash table maintaining the control blocks
        bucket_t* 	_hashTable; 
        
        //pointer to first element in the list of free buffer control blocks
        BCB*        _freeCbList; 
        //Semaphore protecting the list of free buffer control blocks
        Semaphore   _freeCbSem; 
        //number of free buffer control blocks
        uint        _noFreeCbs; 
};


