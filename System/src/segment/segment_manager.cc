#include "segment_manager.hh"

SegmentManager::SegmentManager(PartitionFile& aPartition) :
    // Todo: wie wird der Speicher einer Partition zugewiesen?
    _counterSegmentID(0),
    _segments(),
    _ownPages(),
    _maxSegmentsPerPage(0),
    _partition(aPartition)
{
    _maxSegmentsPerPage = (aPartition.getPageSize() - sizeof(segment_index_header_t)) / sizeof(uint32_t);
}

SegmentManager::~SegmentManager()
{
    for(int i = 0; i < _segments.size(); ++i) {
        delete _segments[i];
    }
}

Segment* SegmentManager::getSegment(const uint aSegmentNo) 
{
    if (!(aSegmentNo < _segments.size())) {
        return 0;
    }
    return _segments[aSegmentNo];
}

Segment* SegmentManager::getNewSegment()
{
    _segments.push_back(new Segment(_counterSegmentID++, _partition));
    return _segments[_segments.size() - 1];
}

const int SegmentManager::storeSegmentManager()
{
     //store all segments
    storeSegments();
     //store yourself
     uint lNoSegments = _segments.size();
     uint lsegmentsCounter =0;
     _partition.openPartition();
     for (uint i=0;i<_ownPages.size();++i){
         //create header
         basic_header_t lBH ={0,_ownPages.at(i),_partition.getID(),1,0,0};
         segment_index_header_t lSMH ={lBH,0,0,1,0};
         if(i<_ownPages.size()-1){
             lSMH._nextIndexPage=_ownPages.at(i+1);
         }
         //else is default

         //write data
         byte* lPageBuffer = new byte[_partition.getPageSize()];
         uint j=0;
         while (j<_maxSegmentsPerPage & lsegmentsCounter<lNoSegments){
             *((uint32_t*) (lPageBuffer + j))=_segments.at(lsegmentsCounter++)->getPageIndex();  //evaluate first, then increment   
             ++j;  
             ++lsegmentsCounter;
         }
         lSMH._noSegments=j+1;
         *(segment_index_header_t*) (lPageBuffer + _partition.getPageSize() - sizeof(segment_index_header_t) )=lSMH;
         _partition.writePage(lPageBuffer,_ownPages.at(i),_partition.getPageSize());          
     }
     _partition.closePartition();
    return 0;
}

const int SegmentManager::loadSegmentManager()
{
     //maxSegmentsPerPage and _partition to be set in constructor

     //load yourself by building a vector of pageIndexes where Segments are stored
     byte* lPageBuffer = new byte[_partition.getPageSize()];  

     basic_header_t lBH ={0,0,_partition.getID(),1,0,0};
     segment_index_header_t lSMH ={lBH,1,0,1,0};
     std::vector<uint32_t> lsegmentPages;

     do{
         _partition.readPage(lPageBuffer,lSMH._nextIndexPage,_partition.getPageSize());
         segment_index_header_t& lSMH=*(segment_index_header_t*) (lPageBuffer+_partition.getPageSize()-sizeof(segment_index_header_t));
         _ownPages.push_back(lSMH._basicHeader._pageIndex);
         for(uint i=0;i<lSMH._noSegments;++i){
             lsegmentPages.push_back(*((uint32_t*) (lPageBuffer + i)));
         }
     }
     while(lSMH._nextIndexPage!=0);

     //load Segments
     uint16_t lmaxSize = (_partition.getPageSize()-sizeof(segment_page_header_t))/4;
    //loadSegments();
     //for each i in lsegmentPages
     for(int i =0; i<lsegmentPages.size();++i){
         //new Segment
        Segment* s = new Segment(0,_partition);
         s->loadSegment(lsegmentPages.at(i));
         _segments.push_back(s);
     }
    return 0;
}

const int SegmentManager::storeSegments()
{
    for(size_t i = 0; i < _segments.size(); ++i)
    {
        _segments[i]->storeSegment();
    }
    return 0;
}

const int SegmentManager::loadSegments()
{
    return -1;
}