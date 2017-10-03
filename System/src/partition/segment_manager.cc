/**
 *  @file    segment_manager.cc
 *  @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *  @brief   A class for managing segments (stored on the disk?)
 *  @bugs    Currently no bugs known
 *  @todos   Implement:
 *              SegmentManager::storeSegmentManager()
 *              SegmentManager::loadSegmentManager()
 *              SegmentManager::storeSegments()
 *              SegmentManager::loadSegments()
 *  @section TBD
 */

#include "segment_manager.hh"
#include "segment.hh"

SegmentManager::SegmentManager() :
    _segments()
{}

SegmentManager::~SegmentManager()
{
    for(int i = 0; i < _segments.size(); ++i) {
        delete _segments.at(i);
    }
    _segments.clear();
}

Segment* SegmentManager::getNewSegment()
{
    Segment* segment = new Segment();
    _segments.add(segment);
    return _segments.at();
}

const int SegmentManager::storeSegmentManager()
{
    //storeSegments();
}

const int SegmentManager::loadSegmentManager()
{
    //loadSegments();
}

const int SegmentManager::storeSegments()
{
    // iterate over vector and serialize every segment first
    //std::vector<Segment*>::iterator it;
    //for(it = _segments.begin; it != _segments.end; ++it) {
    //    it->storeSegment();
    //}
}

const int SegmentManager::loadSegments()
{
    // iterate over vector and deserialize every segment
}