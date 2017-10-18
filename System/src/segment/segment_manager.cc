#include "segment_manager.hh"

SegmentManager::SegmentManager(PartitionBase& aPartition) :
    _counterSegmentID(0),
    _segments(),
    _ownPages(1, aPartition.getSegmentIndexPage()), //one element which is the first page index
    _maxSegmentsPerPage((aPartition.getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t)), //number of pages one segment page can manage
    _partition(aPartition)
{}

SegmentManager::~SegmentManager()
{
    for(uint i = 0; i < _segments.size(); ++i) {
        delete _segments[i];
    }
}

Segment* SegmentManager::createNewSegment()
{
    //TODO ueberlauf von Seiten hier abfangen und neue SgmtMngr Pages allokieren
    Segment* lSegment = new Segment(_counterSegmentID++, _partition);
    _segments[lSegment->getID()] = lSegment;
    return (Segment*)_segments.at(lSegment->getID());
}

int SegmentManager::storeSegmentManager()
{
    std::cout<<"store Segement Manager"<<std::endl;
    _partition.open();
    // store all segments
    storeSegments();
    std::cout<<"stored segements"<<std::endl;
    // store yourself
    auto lsegmentsIterator = _segments.begin();
    //uint lsegmentsCounter = 0;
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    for (uint i = 0; i < _ownPages.size(); ++i) {
        // create header
        // basic header: LSN, PageIndex, PartitionId, Version, unused
        std::cout<< _ownPages.at(i)<<"ownpages PartId "<<_partition.getID()<<std::endl;
        basic_header_t lBH = {0, _ownPages.at(i), _partition.getID(), 1, 0, 0};
        // segment_index_heder: nxtIndexPage, noSegments, version,unused, basicHeader
        segment_index_header_t lSMH = {0, 0, 1, 0, lBH};
        if (i < _ownPages.size() - 1) {
            lSMH._nextIndexPage = _ownPages.at(i + 1);
        }
        // else is default

        // write data
        uint j = 0;
        while ((j < _maxSegmentsPerPage) & (lsegmentsIterator != _segments.end())) {
            *(((uint32_t *)lPageBuffer) + j) = lsegmentsIterator->second->getIndex(); // evaluate first, then increment
            ++j;
            ++lsegmentsIterator;
        }
        lSMH._noSegments = j;
        std::cout<<"no Segments "<<lSMH._noSegments<<std::endl;
        *(segment_index_header_t *)(lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t)) = lSMH;
        _partition.writePage(lPageBuffer, _ownPages.at(i), _partition.getPageSize());
    }
    delete[] lPageBuffer;
    _partition.close();
    return 0;
}

int SegmentManager::loadSegmentManager()
{
    // maxSegmentsPerPage and _partition to be set in constructor

    // load yourself by building a vector of pageIndexes where Segments are stored
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    _partition.open();
    // basic header: LSN, PageIndex, PartitionId, Version, unused
    basic_header_t lBH = {0, 0, _partition.getID(), 1, 0, 0};
    // segment_index_heder: nxtIndexPage, noSegments, version,unused,basicHeader
    segment_index_header_t lSMH = {_partition.getSegmentIndexPage(), 0, 1, 0, lBH};
    std::vector<uint32_t> lSegmentPages;

    do {
        std::cout<<"next Index Page "<<lSMH._nextIndexPage<<std::endl;
        _partition.readPage(lPageBuffer, lSMH._nextIndexPage, _partition.getPageSize());
        // segment_index_header_t &lSMH = *(segment_index_header_t *)(lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t));
        lSMH = *(segment_index_header_t *)(lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t)); //I guess this is what you wanted, Jonas?
        _ownPages.push_back(lSMH._basicHeader._pageIndex);
        for (uint i = 0; i < lSMH._noSegments; ++i) {
            lSegmentPages.push_back(*(((uint32_t *)lPageBuffer) + i));
        }
    } while (lSMH._nextIndexPage != 0);
    loadSegments(lSegmentPages);
    delete[] lPageBuffer;
    _partition.close();
    
    return 0;
}

SegmentBase* SegmentManager::getSegment(const uint16_t aSegmentID)
{
    return _segments.at(aSegmentID);
}

void SegmentManager::storeSegments()
{
    for (auto segmentsItr : _segments)
    {
        segmentsItr.second->storeSegment();   
    }
}

void SegmentManager::loadSegments(uint32_vt& aSegmentPages)
{
    // for each i in lsegmentPages
    for(size_t i = 0; i < aSegmentPages.size(); ++i) 
    {
        Segment* lSegment = new Segment(_partition);
        lSegment->loadSegment(aSegmentPages.at(i));
        _segments[lSegment->getID()] = lSegment;
    }
}