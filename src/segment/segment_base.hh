/**
 *  @file 	segment_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every segment
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#pragma once

#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../partition/partition_base.hh"
#include "../buffer/buf_mngr.hh"
#include "../buffer/buf_cntrl_block.hh"

#include <utility>

class SegmentBase
{
    protected:
        using page_t = std::pair<PID, BCB*>;
        using pages_vt = std::vector<page_t>;

	protected:
		friend class SegmentManager;
		explicit SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
		explicit SegmentBase(PartitionBase& aPartition, const CB& aControlBlock);
        explicit SegmentBase() = delete;
		explicit SegmentBase(const SegmentBase& aSegment) = delete;
        explicit SegmentBase(SegmentBase&&) = delete;
		SegmentBase& operator=(const SegmentBase&) = delete;
        SegmentBase& operator=(SegmentBase&&) = delete;
		virtual ~SegmentBase() = default;

	public:
        /**
         * @brief   Function to request a (logical) page in the given lock mode
         * @param   aPageNo: the (logical) page number
         * @param   aMode: a lock mode to lock the page in
         * @return  pointer to the buffer frame in which the page is located
         * @see     types.hh (LOCK_MODE), buf_mngr.hh
         */
        byte* getPage(const uint aPageNo, LOCK_MODE aMode);

        /**
         * @brief   Writes the page from the buffer pool to its partition (flush call)
         * @param   aPageNo: the (logical) page number to write
         * @see     buf_mngr.hh
         */
		void writePage(const uint aPageNo);

        /**
         * @brief   Release all locks on the page and unfix it
         * @param   aPageNo: the (logical) page number to release
         * @see     buf_mngr.hh
         */
        void releasePage(const uint aPageNo, const bool aModified = false);

        void printPageToFile(uint aPageNo,bool afromDisk = false);

	public:
		virtual PID getNewPage() = 0; // alloc free page, add it to managing vector and return its index in the partition
		inline const PID& getPageID(uint aPageNo){ return _pages.at(aPageNo).first; }

	public:
		inline size_t           getPageSize() noexcept { return _partition.getPageSize(); }
		inline uint16_t         getID() noexcept { return _segID; }
		inline uint32_vt        getIndexPages() noexcept { return _indexPages; }
		/* Return how many pages can be handled by one indexPage. */
		inline int              getIndexPageCapacity() noexcept { return (getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t); }
		inline size_t           getNoPages() noexcept { return _pages.size(); }
		inline PartitionBase&   getPartition() noexcept { return _partition; }
		

	protected:
		virtual void storeSegment() = 0;                          // serialization
		virtual void loadSegment(const uint32_t aPageIndex) = 0;  // deserialization

    private:
        byte* getPageF(const uint aPageNo);
        byte* getPageS(const uint aPageNo);
        byte* getPageX(const uint aPageNo);

	protected:
		/* An ID representing this Segment */
		uint16_t        _segID;
		/* Contains index pages which contain addresses of pages belonging to the segment (for serialization purposes). First element is considered as masterPageIndex */
		uint32_vt       _indexPages;
		/* A map containing all page ID's and their corresponding buffer control block */
        pages_vt        _pages;
		/* Partition the Segment belongs to */
		PartitionBase&  _partition;
        BufferManager&  _bufMan;
        const CB&       _cb;
};
