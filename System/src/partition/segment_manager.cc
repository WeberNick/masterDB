#include "segment_manager.hh"

SegmentManager::SegmentManager(FilePartition& aPartition) :
    _counterSegmentID(0),
    _segments(),
    _ownPages(),
    _maxSegmentsPerPage(0),
    _partition(aPartition)
{
    _maxSegmentsPerPage = (aPartition.getPageSize() - sizeof(segment_index_header_t)) / 4;
}

SegmentManager::~SegmentManager()
{
    for(int i = 0; i < _segments.size(); ++i) {
        delete _segments[i];
    }
}

Segment* SegmentManager::getSegment(const uint aIndex) 
{
    if (!(aIndex < _segments.size())) {
        return 0;
    }
    return _segments[aIndex];
}

Segment* SegmentManager::getNewSegment()
{
    Segment* segment = new Segment(_counterSegmentID++, _partition);
    _segments.push_back(segment);
    return _segments[_segments.size() - 1];
}

const int SegmentManager::storeSegmentManager()
{
    //storeSegments();
    return -1;
}

const int SegmentManager::loadSegmentManager()
{
    //loadSegments();
    return -1;
}

const int SegmentManager::storeSegments()
{
    // iterate over vector and serialize every segment first
    //std::vector<Segment*>::iterator it;
    //for(it = _segments.begin; it != _segments.end; ++it) {
    //    it->storeSegment();
    //}
    return -1;
}

const int SegmentManager::loadSegments()
{
    // iterate over vector and deserialize every segment
    return -1;
}