#include "segment_manager.hh"

SegmentManager::SegmentManager(PartitionBase& aPartition) :
    // Todo: wie wird der Speicher einer Partition zugewiesen?
    _counterSegmentID(0),
    _segments(),
    _ownPages(),
    _maxSegmentsPerPage(0),
    _partition(aPartition)
{
    _maxSegmentsPerPage = (aPartition.getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t);
    _ownPages.push_back(_partition.getSegmentIndexPage());
}

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
    uint lNoSegments = _segments.size();
    auto lsegmentsIterator = _segments.begin();
    //uint lsegmentsCounter = 0;
    byte *lPageBuffer = new byte[_partition.getPageSize()];
    for (uint i = 0; i < _ownPages.size(); ++i) {
        // create header
        // basic header: LSN, PageIndex, PartitionId, Version, unused
        std::cout<< _ownPages.at(i)<<"ownpages PartId "<<_partition.getID()<<std::endl;
        basic_header_t lBH = {0, _ownPages.at(i), _partition.getID(), 1, 0, 0};
        // segment_index_heder: basicHeader, nxtIndexPage, noSegments, version,unused
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

        /*while ((j < _maxSegmentsPerPage) & (lsegmentsCounter < lNoSegments)) {
            _
            *(((uint32_t *)lPageBuffer) + j) = _segments.at(lsegmentsCounter)->getIndex(); // evaluate first, then increment
            ++j;
            ++lsegmentsCounter;
        }*/
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
    std::vector<uint32_t> lsegmentPages;

    do {
        std::cout<<"next Index Page "<<lSMH._nextIndexPage<<std::endl;
        _partition.readPage(lPageBuffer, lSMH._nextIndexPage, _partition.getPageSize());
        lSMH = *(segment_index_header_t *)(lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t));
        _ownPages.push_back(lSMH._basicHeader._pageIndex);
        for (uint i = 0; i < lSMH._noSegments; ++i) {
            lsegmentPages.push_back(*(((uint32_t *)lPageBuffer) + i));
        }
    } while (lSMH._nextIndexPage != 0);

    // uint16_t lmaxSize = (_partition.getPageSize()-sizeof(segment_page_header_t))/4;
    // loadSegments();
    // for each i in lsegmentPages

    //_partition.close();
    for (uint i = 0; i < lsegmentPages.size(); ++i) {
        // new Segment
        Segment *s = new Segment(0,_partition);
        _partition.open();
        s->loadSegment(lsegmentPages.at(i));
        _segments[s->getID()] = s;
    }
    delete[] lPageBuffer;
    _partition.close();
    
    return 0;
}

SegmentBase* SegmentManager::getSegment(const uint16_t aSegmentID)
{
    return _segments.at(aSegmentID);
}

int SegmentManager::storeSegments()
{
    //as there might not be a segment for each index, this might not work
    //for (size_t i = 0; i < _segments.size(); ++i) {
    //    _segments[i]->storeSegment();
    //}
    for ( auto segmentsItr : _segments)
    {
        segmentsItr.second->storeSegment();  
        std::cout<<" store segment "<<segmentsItr.first<<std::endl;  
    }

    return 0;
}

int SegmentManager::loadSegments()
{
    // wird derzeit nicht gebraucht, koennte entweder geloescht werden, oder funktionalitaet wird ausgelagert.
    return -1;
}