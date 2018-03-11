#pragma once

#include "infra/types.hh"



struct page_id_t
{
    uint _fileNo;
    uint _pageNo;
};

struct buffer_acc_cb_t
{
    page_id_t   _pageID;    //
    byte*       _pagePtr;   //
    int         _index      //record within a page (used by caller)
    Semaphore*  _pageSem;   //Semaphor for page
    bool        _modified;  //dirty flag
    bool        _invalid;   //
};

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
    buffer_cb_t*    _chain;         //hash overflow chain forward pointer
};

struct buffer_bucket_t
{
    Semaphore       _bucketSem;     //each bucket is protected by a semaphore while being used
    buffer_cb_t*    _firstBufCb;    //pointer to first control block
};


