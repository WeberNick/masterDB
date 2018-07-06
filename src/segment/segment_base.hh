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
        /**
         * @brief   constructs an entire segment both on disk and the object
         * @param   aSegID          ID of the Segment to be set by SegmentManager
         * @param   aPartition      Partition the segment shall be on
         * @param   aControlBlock   self-explaining
         */
		explicit SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, const CB& aControlBlock);
        /**
         * @brief   only constructs a segment object, no physicall representation is created. Used to load.
         * @param   aPartition      Partition the segment is on
         * @param   aControlBlock   self-explaining
         */
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
        /**
         * @brief   Prints a data page of a segment to file coded in hex
         * @param   aPageNo: the logical page number to print
         * @param   afromDisk: if true, page is directly extracted from disk, if not, the page is buffered.
         */
        void printPageToFile(uint aPageNo,bool afromDisk = false);

	public:
        /**
         * @brief alloc free page, add it to managing vector and return its index in the partition
         * 
         * @return PID the pageID
         */
		virtual PID getNewPage() = 0;
		inline const PID& getPageID(uint aPageNo){ return _pages.at(aPageNo).first; }

	public:
        // Getter
		inline size_t               getPageSize() const noexcept { return _partition.getPageSize(); }
		inline size_t               getPageSize() noexcept { return _partition.getPageSize(); }
		inline uint16_t             getID() const noexcept { return _segID; }
		inline uint16_t             getID() noexcept { return _segID; }
		inline uint32_vt            getIndexPages() const noexcept { return _indexPages; }
		inline uint32_vt            getIndexPages() noexcept { return _indexPages; }
		inline size_t               getNoPages() const noexcept { return _pages.size(); }
		inline size_t               getNoPages() noexcept { return _pages.size(); }
		inline const PartitionBase& getPartition() const noexcept { return _partition; }
		inline PartitionBase&       getPartition() noexcept { return _partition; }
        /**
		 * @brief Return how many pages can be handled by one indexPage.
		 * 
		 * @return int the capacity
		 */
		inline int getIndexPageCapacity() const noexcept { return (getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t); }
		inline int getIndexPageCapacity() noexcept { return static_cast<const SegmentBase&>(*this).getIndexPageCapacity(); }
        /** TODO
         * @brief 
         * 
         * @return std::string 
         */
        inline std::string to_string() const noexcept { return std::string("ID : ") + std::to_string(getID()); }
        inline std::string to_string() noexcept { return static_cast<const SegmentBase&>(*this).to_string(); }

	protected:
        /**
        * @brief   stores the segment to disk in order to shut down the system.
        *          this does not write directly to disk but to buffer.
        */
		virtual void storeSegment() = 0;                          // serialization
        /**
        * @brief   loads the segment from disk on boot.
        * @param   aPageIndex   The page index of first indexPage of the segment. 
        *                       All data can be restored from there on.
        */
		virtual void loadSegment(const uint32_t aPageIndex) = 0;  // deserialization
        /**
        * @brief   erases the segment. This does not destroy the object itself but frees all its pages.
        */
        virtual void erase();                                     // deletes the segment

    private:
        byte* getPageF(const uint aPageNo);
        byte* getPageS(const uint aPageNo);
        byte* getPageX(const uint aPageNo);

	protected:
		uint16_t        _segID;      // An ID representing this Segment
		uint32_vt       _indexPages; // Contains index pages which contain addresses of pages belonging to the segment (for serialization purposes). First element is considered as masterPageIndex
        pages_vt        _pages;      // A vector of pairs containing all data page ID's and their corresponding buffer control block
		PartitionBase&  _partition;  // Partition the Segment belongs to
        BufferManager&  _bufMan;
        const CB&       _cb;
};

std::ostream& operator<< (std::ostream& stream, const SegmentBase& aSegment);
