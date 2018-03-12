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
    private:
        class HashBucket
        {
            public:
                explicit HashBucket() :
                    _bucketSem(0, 1),//mutex
                    _firstBufCb(nullptr)
                {}
                HashBucket(const HashBucket&) = delete;
                HashBucket &operator=(const HashBucket&) = delete;
                ~HashBucket();

            public:
                //each bucket is protected by a semaphore while being used
                Semaphore   _bucketSem;     
                //pointer to first control block
                BCB*        _firstBufCb;    
        };


	public:
		explicit BufferHashTable(const size_t aHashTableSize);
		BufferHashTable(const BufferHashTable&) = delete;
        BufferHashTable &operator=(const BufferHashTable&) = delete;
        ~BufferHashTable();


	private:
		size_t hash(const pid aPageID);


	private:
        //the size of the hash table
		size_t      _size;      		
        //the hash table maintaining the control blocks
        HashBucket* _hashTable; 
        
        //pointer to first element in the list of free buffer control blocks
        BCB*        _freeCbList; 
        //Semaphore protecting the list of free buffer control blocks
        Semaphore   _freeCbSem; 
        //number of free buffer control blocks
        uint        _noFreeCbs; 
};


