#pragma once

#include "buf_types.hh"


class BufferManager
{
    private:
		explicit BufferManager();
        BufferManager(const BufferManager& aBufferManager) = delete;
        BufferManager &operator=(const BufferManager& aBufferManager) = delete;
        ~BufferManager();

	public:
		/**
		 *  @brief  This function is the only way to get access to the BufferManager instance
		 *
		 *  @return reference to the only BufferManager instance
		 */
		static BufferManager& getInstance()
        {
		    static BufferManager lBufMngrInstance;
			return lBufMngrInstance;
		}

    public:
        /* request access to a page and fix it */
        bool fix(const page_id_t aPageID, const LOCK_MODE aMode, buffer_acc_cb_t* const  aBufferAccCbPointer);
        bool emptyfix(const page_id_t aPageID, const LOCK_MODE aMode, buffer_acc_cb_t* const  aBufferAccCbPointer);
        /* unfix a page */
        bool unfix(buffer_acc_cb_t* const aBufferAccCbPointer);
        /* write page to disk */
        bool flush(const buffer_acc_cb_t aBufferAccCb);



    


};
