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

SegmentManager::SegmentManager(Partition& aPartition) :
    _segments(),
    _partition(aPartition)
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

    //store all segments
    
    storeSegments();
    
    //store yourself
    
    uint lNoSegments = _segments.size();
    uint lsegmentsCounter =0;
    for (int i=0;i<_ownPages.size();++i){
        //create header
        basic_header_t lBH ={0,_ownPages.at(i),_partition.getPartitionID(),1,0,0};
        segment_index_header_t lSMH ={lBH,0,0,1,0};
       //        uint32_t _nextIndexPage; //index to the next index page inside this partition, is invalid if set to 0
        //uint16_t _noSegments; //number of managed segments on this physical page only. May be larger for the segment manager
        if(i<_ownPages.size()-1){
            lSMH._nextIndexPage=_ownPages.at(i+1);
        }
        //else is default

        //write data
        byte* lPageBuffer = new byte[_partition.getPageSize()];
        int j=0;
        while (j<_maxSegmentsPerPage & lsegmentsCounter<lNoSegments){
            *((uint32_t*) (lPageBuffer + j))=_segments.at(lsegmentsCounter++).getPageIndex();  //evaluate first, then increment   
            ++j;  
            ++lsegmentsCounter;
        }
        lSMH._noSegments=j+1;
        *(segment_index_header_t*) (lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t) )=lSMH;
        _partition.writePage(lPageBuffer,_ownPages.at(i));          
    }
}

const int SegmentManager::loadSegmentManager()
{
//maxSegmentsPerPage and _partition to be set in constructor

    //load yourself by building a vector of pageIndexes where Segments are stored
    byte* lPageBuffer = new byte[_partition.getPageSize()];  

    basic_header_t lBH ={0,0,_partition.getPartitionID(),1,0,0};
    segment_index_header_t lSMH ={lBH,1,0,1,0};
    std::vector<uint32_t> _segmentPages;

    do{
        _partition.readPage(lSMH._nextIndexPage,lPageBuffer);
        segment_index_header_t& lSMH=*(segment_index_header_t*) (lPageBuffer+_partition.getPageSize()-sizeof(segment_index_header_t));
        _ownPages.push_back(lSMH._basicHeader._pageIndex);
        for(int i=0,i<lSMH._noSegments,++i){
            lsegmentPages.push_back(*((uint32_t*) (lPageBuffer + i)));
        }
    }
    while(lSMH._nextIndexPage!=0)

    //load Segments
    uint16_t lmaxSize = (_partition.getPageSize()-sizeof(segment_page_header_t))/4;
    //loadSegments();
    //for each i in lsegmentPages
    for(int i =0; i<lsegmentPages.size();++i){
        //new Segment
       Segment s = new Segment(_partition);
        s.loadSegment(lsegmentPages.at(i));
        _segments.push_back(&s);
    }

}

const int SegmentManager::storeSegments()
{
    //@Nico/Nick ich vertraue euch mal, dass das funktioniert
    //iterate over vector and serialize every segment first
    std::vector<Segment*>::iterator it;
    for(it = _segments.begin; it != _segments.end; ++it) {
        it->storeSegment();
    }
}

const int SegmentManager::loadSegments()
{
    // iterate over vector and deserialize every segment
    // mMn unnoetig. funktion kann aber auch aus der loadSegmentManager ausgelagert werden
}