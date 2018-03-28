/**
 *  @file 	segment_base.hh
 *  @author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief	A abstract class implementing the interface for every segment
 *  @bugs	Currently no bugs known
 *  @todos	TBD
 *  @section TBD
 */
#ifndef SEGMENT_BASE_HH
#define SEGMENT_BASE_HH

#include "infra/types.hh"
#include "partition/partition_base.hh"
#include "buffer/buf_cntrl_block.hh"
#include "buffer/buf_mngr.hh"

class SegmentBase
{
	protected:
		friend class SegmentManager;
		friend class BufferManager;
		explicit SegmentBase(const uint16_t aSegID, PartitionBase& aPartition, BufferManager& aBufMan);
		explicit SegmentBase(PartitionBase& aPartition, BufferManager& aBufMan);
		SegmentBase(const SegmentBase& aSegment) = delete;
		SegmentBase& operator=(const SegmentBase& aSegment) = delete;
		virtual ~SegmentBase() = 0;

	public:
		int open();
		int close();
		int readPage(byte* aPageBuffer, const uint aPageNo);        // load page from the partition into main memory
		int writePage(const byte* aPageBuffer, const uint aPageNo); // store page from main memory into the partition
		BCB* getPageShared(uint aPageNo);
		BCB* getPageXclusive(uint aPageNo);
		void unfix(BCB* aBCB);


	public:
		virtual int getNewPage() = 0; // alloc free page, add it to managing vector and return its index in the partition
		inline uint32_t getPage(uint aPageNo){ return _pages[aPageNo]; }

	public:
		inline size_t           getPageSize(){ return _partition.getPageSize(); }
		inline uint16_t         getID(){ return _segID; }
		inline uint32_vt        getIndexPages(){ return _indexPages; }
		/* Return how many pages can be handled by one indexPage. */
		inline int              getIndexPageCapacity(){ return (getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t); }
		inline size_t           getNoPages(){ return _pages.size(); }
		inline PartitionBase&   getPartition(){ return _partition; }
		inline byte* 			getFramePtr(BCB* aBCB){return _BufMngr.getFramePtr(aBCB);}
		

	protected:
		virtual int storeSegment() = 0;                          // serialization
		virtual int loadSegment(const uint32_t aPageIndex) = 0;  // deserialization

	protected:
		/* An ID representing this Segment */
		uint16_t _segID;
		/* Contains index pages which contain addresses of pages belonging to the segment (for serialization purposes). First element is considered as masterPageIndex */
		uint32_vt _indexPages;
		/* A vector containing indices to all pages of this segment */
		uint32_vt _pages;
		/* Partition the Segment belongs to */
		PartitionBase& _partition;
		BufferManager& _BufMngr;
};

#endif
