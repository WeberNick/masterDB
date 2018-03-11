/**
 *  @file 	buf_mngr.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Class implementing a access control block for the buffer franes	
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */

#pragma once

#include "infra/types.hh"
#include "infra/semaphore.hh"

class BufferAccessControlBlock
{
    private:
        friend class BufferManager;
        explicit BufferAccessControlBlock();
		BufferAccessControlBlock(const BufferAccessControlBlock&) = delete;
        BufferAccessControlBlock &operator=(const BufferAccessControlBlock&) = delete;
        ~BufferAccessControlBlock();

    public:
        inline pid          getPID(){ return _pageID; }
        inline void         setPID(const pid aPID){ _pageID = aPID; }
        inline byte*        getPagePtr(){ return _pagePtr; }
        inline Semaphore*   getSemaphore(){ return _pageSem; }
        inline bool         getModifiedFlag(){ return _modified; }
        inline bool         getInvalidFlag(){ return _invalid; }



    private:
        pid         _pageID;    //
        byte*       _pagePtr;   //
        Semaphore*  _pageSem;   //Semaphor for page
        bool        _modified;  //dirty flag
        bool        _invalid;   //

};

using BACB = BufferAccessControlBlock;
