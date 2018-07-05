/**
 *  @file 	buf_hash_table.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief 	Class implementieng the hash table of the buffer manager
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "buf_cntrl_block.hh"

#include <cstddef>
#include <functional>
#include <mutex>
#include <shared_mutex>

class BufferHashTable final
{
    private:
        class HashBucket final
        {
            public:
                HashBucket() :
                    _bucketMtx(),
                    _firstBCB(nullptr)
                {}
                explicit HashBucket(const HashBucket&) = delete;
                explicit HashBucket(HashBucket&&) = delete;
                HashBucket& operator=(const HashBucket&) = delete;
                HashBucket& operator=(HashBucket&&) = delete;
                ~HashBucket() = default;

            public:
                inline sMtx&    getMtx() noexcept { return _bucketMtx; }
                inline BCB*     getBCB() noexcept { return _firstBCB; }
                inline void     setBCB(BCB* aBCB) noexcept { _firstBCB = aBCB; }

            private:
                //each bucket is protected by a mutex while being used
                sMtx    _bucketMtx;     
                //pointer to first control block
                BCB*    _firstBCB;    
        };


	public:
        BufferHashTable() = delete;
		explicit BufferHashTable(const size_t aHashTableSize);
		explicit BufferHashTable(const BufferHashTable&) = delete;
        explicit BufferHashTable(BufferHashTable&&) = delete;
        BufferHashTable& operator=(const BufferHashTable&) = delete;
        BufferHashTable& operator=(BufferHashTable&&) = delete;
        ~BufferHashTable();

    public:
        inline size_t   getTableSize() noexcept { return _size; }
        inline sMtx&    getBucketMtx(const size_t aHash);
        inline BCB*     getBucketBCB(const size_t aHash) noexcept { return _hashTable[aHash].getBCB(); }
        inline void     setBucketBCB(const size_t aHash, BCB* aBCB) noexcept { _hashTable[aHash].setBCB(aBCB); }
        std::vector<BCB*> getAllValidBCBs(); //get all BCBs from all Buckets in order to flush them

	public:
		size_t hash(const PID& aPageID) noexcept;


	private:
        //the size of the hash table
		size_t          _size;      		
        //the hash table maintaining the control blocks
        std::unique_ptr<HashBucket[]>   _hashTable;
};

sMtx&    BufferHashTable::getBucketMtx(const size_t aHash) noexcept
{ 
	TRACE("got Bucket Mutex of "+std::to_string(aHash));
	return _hashTable[aHash].getMtx(); 
}