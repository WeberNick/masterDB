#pragma once

#include "infra/types.hh"
#include "infra/semaphore.hh"

#include <cstddef>

struct buffer_cb_t
{
    page_id_t       _pageID;        //no. of page in buffer(0 = no page)
    //page handle?
    uint            _frameIndex;    //buffer pool index where page now stored
    Semaphore       _pageSem;       //semaphore to protect page
    bool            _modified;
    int             _fixCount;
    //LSN infos
    buffer_cb_t*    _prevInLRU;     //prev. page in LRU chain
    buffer_cb_t*    _nextInLRU;     //next page in LRU chain
    buffer_cb_t*    _nextInChain;   //hash overflow chain forward pointer
};

struct buffer_acc_cb_t
{
    page_id_t   _pageID;    //
    byte*       _pagePtr;   //
    int         _index;     //record within a page (used by caller)
    Semaphore*  _pageSem;   //Semaphor for page
    bool        _modified;  //dirty flag
    bool        _invalid;   //
};

struct buffer_bucket_t
{
    //each bucket is protected by a semaphore while being used
    Semaphore       _bucketSem;     
    //pointer to first control block
    buffer_cb_t*    _firstBufCb;    
};

class BufferHashTable
{
	public:
		explicit BufferHashTable(const std::size_t aHashTableSize);
		BufferHashTable(const BufferHashTable& aBufferHashTable) = delete;
        BufferHashTable &operator=(const BufferHashTable& aBufferHashTable) = delete;
        ~BufferHashTable();


	private:
		std::size_t hash(const page_id_t aPageID);


	private:
        //the size of the hash table
		std::size_t 		_size;      		
        //the hash table maintaining the control blocks
        buffer_bucket_t* 	_hashTable; 
        
        //pointer to first element in the list of free buffer control blocks
        buffer_cb_t*        _freeCbList; 
        //Semaphore protecting the list of free buffer control blocks
        Semaphore           _freeCbSem; 
        //number of free buffer control blocks
        uint                _noFreeCbs; 
};

class BufferManager
{
    public:
		explicit BufferManager(const std::size_t aNoFrames, const std::size_t aFrameSize = 4096);
        BufferManager(const BufferManager& aBufferManager) = delete;
        BufferManager &operator=(const BufferManager& aBufferManager) = delete;
        ~BufferManager();

    public:
        /* request access to a page and fix it */
        bool fix(const page_id_t aPageID, const LOCK_MODE aMode, buffer_acc_cb_t* const  aBufferAccCbPointer);
        bool emptyfix(const page_id_t aPageID, const LOCK_MODE aMode, buffer_acc_cb_t* const  aBufferAccCbPointer);
        /* unfix a page */
        bool unfix(buffer_acc_cb_t* const aBufferAccCbPointer);
        /* write page to disk */
        bool flush(const buffer_acc_cb_t aBufferAccCb);


	private:
		std::size_t 		_noFrames;
		std::size_t 		_frameSize;;
		BufferHashTable* 	_bufferHash;
		byte** 				_bufferpool;
		uint*				_freeFrameIndexes;
		Semaphore 			_freeFrameSem;
		unsigned long 		_noFreeFrames;

		//pointer to control block of most recently used page
        buffer_cb_t*        _pageMRU;   
        //pointer to control block of last recently used page
        buffer_cb_t*        _pageLRU;           
        //Semaphore protecting lru  list
        Semaphore           _LRUSem;    

 	 	//pointer to the first element in the list of free buffer access control blocks
        buffer_acc_cb_t*    _freeAccCbList;
        //semaphore to protect operations on the list of free access control blocks
        Semaphore           _freeAccCbSem;
        //current number of free access control blocks
        uint                _noFreeAccCb;


};
