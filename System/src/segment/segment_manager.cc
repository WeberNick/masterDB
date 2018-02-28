#include "segment_manager.hh"

SegmentManager::SegmentManager() :
    _counterSegmentID(0),
    _segments(),
	_segmentsByID(),
	_segmentsByName(),
	_segmentTuples(),
    _indexPages(), // one element which is the first page index??
    /* REMOVE THIS MAGIC NUMBER AS SOON A SOLUTION IS FOUND */
    _maxSegmentsPerPage((4096 - sizeof(segment_index_header_t)) / sizeof(uint32_t)) //number of pages one segment page can manage
{}

SegmentManager::~SegmentManager()
{
    for(uint i = 0; i < _segments.size(); ++i) {
        delete _segments[i];
    }
}

void SegmentManager::load(seg_vt& aTuples)
{
    _segmentTuples = aTuples;
    //fill internal data structure with all relevant info
    for(auto& segTuple : _segmentTuples)
    {
      _segmentsByID[segTuple._sID] = &segTuple;
      _segmentsByName[segTuple._sName] = &segTuple;
    }
}

void SegmentManager::store(PartitionFile& aMasterPartition, const uint aSegmentIndex)
{
    SegmentFSM_SP lMasterSeg(aMasterPartition);
    lMasterSeg.loadSegment(aSegmentIndex);
    int lFreeBytesPerPage = lMasterSeg.getPageSize() - sizeof(segment_fsm_sp_header_t) -sizeof(sp_header_t);
    //get size of master segment
    int lCapazIst = lMasterSeg.getNoPages() * ( lFreeBytesPerPage /sizeof(seg_t) ) ;
    //get number of segments
    // estimate if size is big enough, if not add new pages
    int lCapazSoll = _segmentTuples.size()/sizeof(seg_t);
    if(lCapazIst-lCapazSoll < 0){
        //how many new pages?
        int a =  (int) ceil(( lCapazSoll-lCapazIst) / ( lFreeBytesPerPage /sizeof(seg_t) ));
        while(a>0){
            //alloc new pages
            lMasterSeg.getNewPage();
            ++a;
        }

    }
    //write all tuples that were not deleted (condition to be hold in delete segment method)
    //get new tuples (tuples always up to date)
    //delete all content of pages
    //write everything on segment
    byte* lPage = new byte[aMasterPartition.getPageSize()];
    InterpreterSP lInterpreter;
    uint lSegCounter = 0;
    byte* lPos;
    for (uint i = 0; i < lMasterSeg.getNoPages(); ++i)
    {
   	    lMasterSeg.readPage(lPage, 0);
        lInterpreter.attach(lPage);
        lInterpreter.initNewPage(lPage);   
        while(true){
           lPos = lInterpreter.addNewRecord(sizeof(seg_t));
           if (lPos == 0){
               break;
           }
            *((seg_t*) lPos ) =  _segmentTuples.at(lSegCounter);
            ++lSegCounter;
        }   
    }

}

SegmentFSM* SegmentManager::createNewSegmentFSM(PartitionBase& aPartition, std::string aName)
{
    SegmentFSM* lSegment = new SegmentFSM(_counterSegmentID++, aPartition);
    _segments[lSegment->getID()] = lSegment;
    seg_t lSegT ={aPartition.getID(), lSegment->getID(),	aName,1,  lSegment->getIndexPages().at(0) };
    _segmentTuples.push_back(lSegT);
    _segmentsByID[lSegT._sID]=&_segmentTuples[_segmentTuples.size()-1];
    _segmentsByName[lSegT._sName]=&_segmentTuples[_segmentTuples.size()-1];
    return (SegmentFSM*)_segments.at(lSegment->getID());
}
SegmentFSM_SP* SegmentManager::createNewSegmentFSM_SP(PartitionBase& aPartition, std::string aName)
{
    SegmentFSM_SP* lSegment = new SegmentFSM_SP(_counterSegmentID++, aPartition);
    _segments[lSegment->getID()] = lSegment;
    seg_t lSegT ={aPartition.getID(), lSegment->getID(),	aName,2,  lSegment->getIndexPages().at(0)};
    _segmentTuples.push_back(lSegT);
    _segmentsByID[lSegT._sID]=&_segmentTuples[_segmentTuples.size()-1];
    _segmentsByName[lSegT._sName]=&_segmentTuples[_segmentTuples.size()-1];
    return (SegmentFSM_SP*)_segments.at(lSegment->getID());
}

SegmentFSM_SP* SegmentManager::loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex)
{
  SegmentFSM_SP* lSegment = new SegmentFSM_SP(aPartition);
  lSegment->loadSegment(aIndex);
  return lSegment;
}

void SegmentManager::deleteSegment(SegmentBase* aSegment)
{
  delete aSegment;
}

void SegmentManager::deleteSegment(const uint16_t aID)
{
    //todo
}

void SegmentManager::deleteSegment(const std::string aName)
{
    //todo
}

int SegmentManager::storeSegmentManager(PartitionBase& aPartition)
{
    //TODO: use code that can be reused, delete rest
    std::cout<<"store Segement Manager"<<std::endl;
    aPartition.open();
    // store all segments
    storeSegments();
    std::cout<<"stored segements"<<std::endl;
    // store yourself
    auto lsegmentsIterator = _segments.begin();
    //uint lsegmentsCounter = 0;
    byte *lPageBuffer = new byte[aPartition.getPageSize()];
    for (uint i = 0; i < _indexPages.size(); ++i) {
        // create header
        // basic header: LSN, PageIndex, PartitionId, Version, unused
        std::cout<< _indexPages.at(i)<<"ownpages PartId "<<aPartition.getID()<<std::endl;
        basic_header_t lBH = {0, _indexPages.at(i), aPartition.getID(), 1, 0, 0};
        // segment_index_heder: nxtIndexPage, noSegments, version,unused, basicHeader
        segment_index_header_t lSMH = {0, 0, 1, 0, lBH};
        if (i < _indexPages.size() - 1) {
            lSMH._nextIndexPage = _indexPages.at(i + 1);
        }
        // else is default

        // write data
        uint j = 0;
        while ((j < _maxSegmentsPerPage) & (lsegmentsIterator != _segments.end())) {
            *(((uint32_t *)lPageBuffer) + j) = lsegmentsIterator->second->getIndexPages()[0]; // evaluate first, then increment
            ++j;
            ++lsegmentsIterator;
        }
        lSMH._noSegments = j;
        std::cout<<"no Segments "<<lSMH._noSegments<<std::endl;
        *(segment_index_header_t *)(lPageBuffer + aPartition.getPageSize() - sizeof(segment_index_header_t)) = lSMH;
        aPartition.writePage(lPageBuffer, _indexPages.at(i), aPartition.getPageSize());
    }
    delete[] lPageBuffer;
    aPartition.close();
    return 0;
}

int SegmentManager::loadSegmentManager(PartitionBase& aPartition)
{
    //TODO: use code that can be reused, delete rest


    // maxSegmentsPerPage and aPartition to be set in constructor

    // load yourself by building a vector of pageIndexes where Segments are stored
    byte *lPageBuffer = new byte[aPartition.getPageSize()];
    aPartition.open();
    // basic header: LSN, PageIndex, PartitionId, Version, unused
    basic_header_t lBH = {0, 0, aPartition.getID(), 1, 0, 0};
    // segment_index_heder: nxtIndexPage, noSegments, version,unused,basicHeader
    segment_index_header_t lSMH = {/*aPartition.getSegmentIndexPage()*/0, 0, 1, 0, lBH};
    std::vector<uint32_t> lSegmentPages;

    do {
        std::cout<<"next Index Page "<<lSMH._nextIndexPage<<std::endl;
        aPartition.readPage(lPageBuffer, lSMH._nextIndexPage, aPartition.getPageSize());
        // segment_index_header_t &lSMH = *(segment_index_header_t *)(lPageBuffer + aPartition.getPageSize() - sizeof(segment_index_header_t));
        lSMH = *(segment_index_header_t *)(lPageBuffer + aPartition.getPageSize() - sizeof(segment_index_header_t)); //I guess this is what you wanted, Jonas?
        _indexPages.push_back(lSMH._basicHeader._pageIndex);
        for (uint i = 0; i < lSMH._noSegments; ++i) {
            lSegmentPages.push_back(*(((uint32_t *)lPageBuffer) + i));
        }
    } while (lSMH._nextIndexPage != 0);
    loadSegments(lSegmentPages, aPartition);
    delete[] lPageBuffer;
    aPartition.close();
    
    return 0;
}

SegmentBase* SegmentManager::getSegment(const uint16_t aSegmentID)
{
    //if the object has not been created before, create it and store it
    if (_segments.find(aSegmentID)==_segments.end()) {
        //find out which type
        seg_t lTuple = *_segmentsByID[aSegmentID];
        PartitionManager& partMngr = PartitionManager::getInstance();
        PartitionBase& part = *(partMngr.getPartition(lTuple._sPID));
        SegmentBase* s;
        switch(lTuple._sType){
            //DOES NOT WORK BECAUSE: partition muss noch geladen werden, und zwar die, auf der Segment steht.
            case 1://SegmentFSM
            s = new SegmentFSM(part);
            break;
            case 2: //segmentFSM NSM
            s = new SegmentFSM_SP(part);
            break;
            //more to come

            default: return nullptr;
        }
        s->loadSegment(lTuple._sFirstPage);
        _segments[lTuple._sID]=s;
    }
    //now it is created and can be retrieved
    //else it is in the map, you can just pass it
    return _segments[aSegmentID];
}

void SegmentManager::storeSegments()
{
    for (auto segmentsItr : _segments)
    {
        segmentsItr.second->storeSegment();   
    }
}

void SegmentManager::loadSegments(uint32_vt& aSegmentPages, PartitionBase& aPartition)
{
    // for each i in lsegmentPages
    for(size_t i = 0; i < aSegmentPages.size(); ++i) 
    {
        Segment* lSegment = new Segment(aPartition);
        lSegment->loadSegment(aSegmentPages.at(i));
        _segments[lSegment->getID()] = lSegment;
    }
}
