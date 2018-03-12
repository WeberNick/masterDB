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
        inline void         setPID(pid aPageID){ _pageID = aPageID; }
        inline byte*        getPagePtr(){ return _pagePtr; }
        inline void         setPagePtr(byte* aPagePtr){ _pagePtr = aPagePtr; }
        inline Semaphore*   getSemaphore(){ return _pageSem; }
        inline void         setSemaphore(Semaphore* aSemaphore){ _pageSem = aSemaphore; }
        inline bool         getModifiedFlag(){ return _modified; }
        inline void         setModifiedFlag(const bool aFlag){ _modified = aFlag; }
        inline bool         getInvalidFlag(){ return _invalid; }
        inline void         setInvalidFlag(const bool aFlag){ _invalid = aFlag; }



    private:
        pid         _pageID;    //
        byte*       _pagePtr;   //
        Semaphore*  _pageSem;   //Semaphor for page
        bool        _modified;  //dirty flag
        bool        _invalid;   //

};

using BACB = BufferAccessControlBlock;
