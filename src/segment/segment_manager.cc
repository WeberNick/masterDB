#include "segment_manager.hh"

SegmentManager::SegmentManager() :
    _counterSegmentID(0),
    _segments(),
	_segmentsByID(),
	_segmentsByName(),
	_segmentTuples(),
    _indexPages(), // one element which is the first page index??
    /* REMOVE THIS MAGIC NUMBER AS SOON A SOLUTION IS FOUND */
    _maxSegmentsPerPage((4096 - sizeof(segment_index_header_t)) / sizeof(uint32_t)), //number of pages one segment page can manage
    _BufMngr( BufferManager::getInstance()),
    _masterSegSegName("SegmentMasterSegment"),
    _cb(nullptr),
    _init(false)
{   }


SegmentManager::~SegmentManager()
{
    for(uint i = 0; i < _segments.size(); ++i) {
        delete _segments[i];
    }
}

void SegmentManager::init(const CB& aControlBlock)
{
    if(!_init)
    {
        _cb = &aControlBlock;
        _init = true;
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

SegmentFSM* SegmentManager::createNewSegmentFSM(PartitionBase& aPartition, std::string aName)
{
    SegmentFSM* lSegment = new SegmentFSM(_counterSegmentID++, aPartition, *_cb);
    _segments[lSegment->getID()] = lSegment;
    seg_t lSegT ={aPartition.getID(), lSegment->getID(),	aName,1,  lSegment->getIndexPages().at(0) };
    createSegmentSub(lSegT);
    const std::string lErrMsg("Created new Segment FSM successfully.");
    if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
    return (SegmentFSM*)_segments.at(lSegment->getID());
}
SegmentFSM_SP* SegmentManager::createNewSegmentFSM_SP(PartitionBase& aPartition, std::string aName)
{
    SegmentFSM_SP* lSegment = new SegmentFSM_SP(_counterSegmentID++, aPartition, *_cb);
    _segments[lSegment->getID()] = lSegment;
    seg_t lSegT ={aPartition.getID(), lSegment->getID(),	aName,2,  lSegment->getIndexPages().at(0)};
    createSegmentSub(lSegT);
    const std::string lErrMsg("Created new Segment FSM SP successfully.");
    if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
    return (SegmentFSM_SP*)_segments.at(lSegment->getID());
}
void SegmentManager::createSegmentSub(seg_t aSegT){
    _segmentTuples.push_back(aSegT);
    _segmentsByID[aSegT._sID]=&_segmentTuples[_segmentTuples.size()-1];
    _segmentsByName[aSegT._sName]=&_segmentTuples[_segmentTuples.size()-1];

    SegmentFSM_SP* lSegments = (SegmentFSM_SP*) getSegment(_segmentsByName[_masterSegSegName]->_sID);
    lSegments->insertTuple((byte*) &aSegT,sizeof(seg_t));
}

SegmentFSM_SP* SegmentManager::loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex)
{
  SegmentFSM_SP* lSegment = new SegmentFSM_SP(aPartition,*_cb);
  lSegment->loadSegment(aIndex);
  return lSegment;
}
 

void SegmentManager::deleteSegment(SegmentBase* aSegment)
{
  delete aSegment;
}

void SegmentManager::deleteSegment(const uint16_t aID)
{
    //delete object if exists
    auto segIter =  _segments.find(aID);
    if( segIter !=_segments.end()){
        delete segIter->second;
        _segments.erase(segIter);
    }
    seg_t* seg = _segmentsByID[aID];
    //delete tuple on disk
    deleteTupelPhysically(_masterSegSegName,aID,0);

    //delete tuple in memory
    _segmentsByName.erase(seg->_sName);
    _segmentsByID.erase(aID);

    for (auto segIter2 = _segmentTuples.begin(); segIter2 <_segmentTuples.end(); ++segIter2){
        if (   seg->_sPID == segIter2->_sPID && 
        seg->_sID==segIter2->_sID && 
        seg->_sType == segIter2->_sType &&
        seg->_sFirstPage ==segIter2->_sFirstPage &&
        (seg->_sName.compare(segIter2->_sName)==0)){
            _segmentTuples.erase(segIter2);
            break;
        }
    }
    const std::string lErrMsg("Deleted segment successfully.");
    if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }  }

void SegmentManager::deleteSegment(const std::string aName)
{
    //get ID and delete by ID
    deleteSegment(_segmentsByName[aName]->_sID);
}

int SegmentManager::deleteTupelPhysically (std::string aMasterName, uint16_t aID, uint8_t aType){
    //type=0 if segment, type=1 if partition

    //open master Segment by name and load it
    SegmentFSM_SP* lSegments = (SegmentFSM_SP*) getSegment(_segmentsByName[aMasterName]->_sID);
    byte* lPage;
    InterpreterSP lInterpreter;
    BCB* lBCB;

    //search all pages for tuple
    uint j;
    for (uint i = 0; i < lSegments->getNoPages(); ++i)
    {
    /*####################################################################*/
      //lBCB = lSegments->getPageShared(i); //now only call to getPage with lock mode
      //lPage = lSegments->getFramePtr(lBCB);
    /*####################################################################*/

   	  lInterpreter.attach(lPage);
      j=0;
   	  while( j < lInterpreter.noRecords())
   	  {
        if( deleteTypeChecker( lInterpreter.getRecord(j),aID,aType) ){
            //mark deleted
            lBCB->getMtx().unlock_shared();
            lBCB->getMtx().lock();
            lInterpreter.deleteRecordSoft(j);
            lBCB->setModified(true);
            lBCB->getMtx().unlock();
    /*####################################################################*/
    //lSegments->unfix(lBCB);
    /*####################################################################*/
            const std::string lErrMsg("Tuple deleted successfully.");
            if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }  
            return 1;
        }
        ++j;
   	  }
    lBCB->getMtx().unlock_shared();
    /*####################################################################*/
    //lSegments->unfix(lBCB);
    /*####################################################################*/
    }
    //tuple not found, rtn -1
    const std::string lErrMsg("Deletion of tuple went wrong - tuple not found.");
    if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
    return -1;
}
bool SegmentManager::deleteTypeChecker ( byte* aRecord,uint16_t aID,uint8_t aType){
    if(aType==0){//segment
       return ((seg_t*) aRecord)->_sID == aID;
    }
    else if (aType==1){//partitions
       return ((part_t*) aRecord)->_pID == aID;
    }
    else{
        return false;
    }
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
            s = new SegmentFSM(part,*_cb);
            break;
            case 2: //segmentFSM NSM
            s = new SegmentFSM_SP(part,*_cb);
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

SegmentBase* SegmentManager::getSegment(const std::string aSegmentName){
    return (SegmentBase*) getSegment(_segmentsByName[aSegmentName]->_sID);
}

void SegmentManager::storeSegments()
{
    for (auto segmentsItr : _segments)
    {
        segmentsItr.second->storeSegment();   
    }
}

void SegmentManager::createMasterSegments(PartitionFile* aPartition, const std::string& aName){
   //create 2 Master Segments
   //MasterSegParts
     SegmentFSM_SP* lPSeg = new SegmentFSM_SP(_counterSegmentID++, *aPartition, *_cb);
    _segments[lPSeg->getID()] = lPSeg;
    seg_t lPSegT ={aPartition->getID(), lPSeg->getID(),	aName, 2,  lPSeg->getIndexPages().at(0)};
    //MasterSegSegs
      SegmentFSM_SP* lSSeg = new SegmentFSM_SP(_counterSegmentID++, *aPartition, *_cb);
    _segments[lSSeg->getID()] = lSSeg;
    seg_t lSSegT ={aPartition->getID(), lSSeg->getID(),	_masterSegSegName, 2,  lSSeg->getIndexPages().at(0)};

    //store them into Segment Master
    createSegmentSub(lPSegT);
    createSegmentSub(lSSegT);
    const std::string lErrMsg("Created master segments successfully.");
    if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
}


//old code
/*
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
    segment_index_header_t lSMH = {aPartition.getSegmentIndexPage()0, 0, 1, 0, lBH};
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
*/

