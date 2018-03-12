/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the hash table of the buffer manager
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#ifndef BUFFER_HASH_TABLE_HH
#define BUFFER_HASH_TABLE_HH
#pragma once

#include "infra/types.hh"
#include "buf_cntrl_block.hh"

#include <cstddef>
#include <functional>
#include <mutex>
#include <shared_mutex>

class BufferHashTable
{
    private:
        class HashBucket
        {
            public:
                explicit HashBucket() :
                    _bucketMutex(),
                    _firstBufCb(nullptr)
                {}
                HashBucket(const HashBucket&) = delete;
                HashBucket &operator=(const HashBucket&) = delete;
                ~HashBucket(){}

            public:
                //each bucket is protected by a mutex while being used
                sMtx    _bucketMutex;     
                //pointer to first control block
                BCB*    _firstBufCb;    
        };


	public:
		explicit BufferHashTable(const size_t aHashTableSize);
		BufferHashTable(const BufferHashTable&) = delete;
        BufferHashTable &operator=(const BufferHashTable&) = delete;
        ~BufferHashTable();

    public:
        inline sMtx&    getBucketMutex(const size_t aHash){ return _hashTable[aHash]._bucketMutex; }
        inline BCB*     getBucketCb(const size_t aHash){ return _hashTable[aHash]._firstBufCb; }



	public:
		size_t hash(const pid aPageID);


	private:
        //the size of the hash table
		size_t          _size;      		
        //the hash table maintaining the control blocks
        HashBucket*     _hashTable; 
        
        //pointer to first element in the list of free buffer control blocks
        BCB*            _freeCbList; 
        //Semaphore protecting the list of free buffer control blocks
        sMtx            _freeCbMutex; 
        //number of free buffer control blocks
        uint            _noFreeCbs; 
};


#endif
