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
                    _bucketMtx(),
                    _firstBCB(nullptr)
                {}
                HashBucket(const HashBucket&) = delete;
                HashBucket(HashBucket&&) = delete;
                HashBucket& operator=(const HashBucket&) = delete;
                HashBucket& operator=(HashBucket&&) = delete;
                ~HashBucket(){}

            public:
                inline sMtx&    getMtx(){ return _bucketMtx; }
                inline BCB*     getBCB(){ return _firstBCB; }
                inline void     setBCB(BCB* aBCB){ _firstBCB = aBCB; }

            private:
                //each bucket is protected by a mutex while being used
                sMtx    _bucketMtx;     
                //pointer to first control block
                BCB*    _firstBCB;    
        };


	public:
		explicit BufferHashTable(const size_t aHashTableSize);
		BufferHashTable(const BufferHashTable&) = delete;
        BufferHashTable(BufferHashTable&&) = delete;
        BufferHashTable& operator=(const BufferHashTable&) = delete;
        BufferHashTable& operator=(BufferHashTable&&) = delete;
        ~BufferHashTable();

    public:
        inline size_t   getTableSize(){ return _size; }
        inline sMtx&    getBucketMtx(const size_t aHash){ return _hashTable[aHash].getMtx(); }
        inline BCB*     getBucketBCB(const size_t aHash){ return _hashTable[aHash].getBCB(); }
        inline void     setBucketBCB(const size_t aHash, BCB* aBCB){ _hashTable[aHash].setBCB(aBCB); }

	public:
		size_t hash(const pid aPageID);

	private:
        //the size of the hash table
		size_t          _size;      		
        //the hash table maintaining the control blocks
        HashBucket*     _hashTable; 
};
