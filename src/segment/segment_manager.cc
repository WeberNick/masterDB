#include "segment_manager.hh"

SegmentManager::SegmentManager() :
    _counterSegmentID(0),
    _segments(),
	_segmentsByID(),
	_segmentsByName(),
    _masterSegSegName("SegmentMasterSegment"),
    _BufMngr( BufferManager::getInstance()),
    _cb(nullptr)
{
    TRACE("'SegementManager' constructed");
}

SegmentManager::~SegmentManager()
{
    for(const auto& elem : _segments)
    {
        delete elem.second;
    }
    TRACE("'SegementManager' destructed");
}

void SegmentManager::init(const CB& aControlBlock) noexcept
{
    if(!_cb)
    {
        _cb = &aControlBlock;
    TRACE("'SegementManager' initialized");
    }
}

void SegmentManager::load(const seg_vt& aTuples) noexcept
{
    // fill internal data structure with all relevant info
    uint16_t maxID = 0; //trying to find the maximal ID already assigned to a segment
    for(const auto& segTuple : aTuples)
    {
    std::cout<<segTuple<<std::endl;
    //insert tuples into data structures
      _segmentsByID[segTuple.ID()] = segTuple;
      _segmentsByName[segTuple.name()] = segTuple.ID();
      TRACE(segTuple.to_string() + std::string(" (DELETE TRACE AFTER DEBUGGING)"));
      if( maxID <= segTuple.ID()){
          maxID = segTuple.ID();
      }
    }
    _counterSegmentID = maxID+1;
}

SegmentFSM* SegmentManager::createNewSegmentFSM(PartitionBase& aPartition, const std::string& aName)
{
    TRACE("Request to create FSM segment in partition '" + aPartition.to_string() + "' with name '" + aName + "'");
    SegmentFSM* lSegment = new SegmentFSM(_counterSegmentID++, aPartition, *_cb);
    _segments[lSegment->getID()] = lSegment;

    Segment_T lSegT(aPartition.getID(), lSegment->getID(), aName, 1, lSegment->getIndexPages().at(0));

    createSegmentSub(lSegT);
    TRACE("FSM segment created and successfully added to the SegmentManager");
    return static_cast<SegmentFSM*>(getSegment(lSegment->getID()));
}

SegmentFSM_SP* SegmentManager::createNewSegmentFSM_SP(PartitionBase& aPartition, const std::string& aName)
{
    TRACE("Request to create FSM_SP segment in partition '" + aPartition.to_string() + "' with name '" + aName + "'");
    if (_segmentsByName.find(aName) != _segmentsByName.end()) {
        throw SegmentExistsException(FLF);
    }
    SegmentFSM_SP* lSegment = new SegmentFSM_SP(_counterSegmentID++, aPartition, *_cb);
    _segments[lSegment->getID()] = lSegment;
    Segment_T lSegT(aPartition.getID(), lSegment->getID(), aName, 2, lSegment->getIndexPages().at(0));

    createSegmentSub(lSegT);
    TRACE("FSM_SP segment created and successfully added to the SegmentManager");
    return static_cast<SegmentFSM_SP*>(getSegment(lSegment->getID()));
}

void SegmentManager::createSegmentSub(const Segment_T& aSegT){
    TRACE(std::string("(DELETE TRACE AFTER DEBUGGING) trying to insert the following tuple:\n") +aSegT.to_string() );
    //insert into maps
    _segmentsByID[aSegT.ID()] = aSegT;
    _segmentsByName[aSegT.name()] = aSegT.ID();
    //insert tuple on disk
    SegmentFSM_SP* lSegments = (SegmentFSM_SP*)getSegment(_masterSegSegName);
    lSegments->insertTuple<Segment_T>(aSegT);
}

SegmentFSM_SP* SegmentManager::loadSegmentFSM_SP(PartitionBase& aPartition, const uint aIndex)
{
  SegmentFSM_SP* lSegment = new SegmentFSM_SP(aPartition,*_cb);
  lSegment->loadSegmentUnbuffered(aIndex);
  return lSegment;
}
 

void SegmentManager::deleteSegment(SegmentFSM_SP*& aSegment)
{
    delete aSegment;
    aSegment = nullptr;
}

void SegmentManager::deleteSegment(const uint16_t aID)
{
    //@Nick: bitte nicht im TRACE getSegment nehmen. getSegment läd schon und kann abstürzen.
    TRACE("Deletion of Segment " + std::to_string(aID) + " starts...");
    SegmentBase* lSeg = getSegment(aID);
    lSeg->erase(); //free all pages
    // delete object itself, which now exists.
    delete lSeg; 
    _segments.erase(aID);

    const Segment_T seg(_segmentsByID.at(aID));
    // delete tuple on disk
    deleteTuplePhysically<Segment_T>(_masterSegSegName,aID);

    // delete tuple in memory
    _segmentsByName.erase(seg.name());
    _segmentsByID.erase(aID);
    TRACE("Deletion of segment successfully finished.");
}

void SegmentManager::deleteSegment(const std::string& aName)
{
    // get ID and delete by ID
    try
    {
        deleteSegment(_segmentsByName.at(aName));
    }
    catch(const std::out_of_range& oor)
    {
        throw SegmentNotExistsException(FLF);
    }
}

void SegmentManager::deleteSegements(const uint8_t aPartitionID) {
    //@Nick kann der Aufruf mehr als einen Treffer finden? Es gibt nämlich potentiell tausende.
    auto america = std::find_if(_segmentsByID.cbegin(), _segmentsByID.cend(), [aPartitionID] (const auto& elem) { return elem.second.partID() == aPartitionID; }); 
    if(america != _segmentsByID.cend())
    {
        deleteSegment(america->first);
    }
    //for (auto& america : _segmentsByID)
    //{
        //if (america.second.partID() == aPartitionID)
        //{
            //deleteSegment(america.first);
        //}
    //}
}

SegmentBase* SegmentManager::getSegment(const uint16_t aSegmentID)
{
    // if the object has not been created before, create it and store it
    if (_segments.find(aSegmentID) == _segments.end())
    {
        TRACE("Trying to load the segment from disk.");
        //find out which type
        const Segment_T lTuple(_segmentsByID.at(aSegmentID));
        PartitionManager& partMngr = PartitionManager::getInstance();
        PartitionBase& part = *(partMngr.getPartition(lTuple.partID()));
        SegmentBase* s;
        switch (lTuple.type())
        {
            case 1: // SegmentFSM
                s = new SegmentFSM(part, *_cb);
                break;
            case 2: // SegmentFSM NSM
                s = new SegmentFSM_SP(part, *_cb);
                break;
            // more to come
            default:
                return nullptr;
        }
        s->loadSegment(lTuple.firstPage());
        _segments[lTuple.ID()] = s;
    }
    // now it is created and can be retrieved
    // else it is in the map, you can just pass it
    TRACE("found the segment, SegmentID " + std::to_string(aSegmentID));
    return _segments.at(aSegmentID);
}

SegmentBase* SegmentManager::getSegment(const std::string& aSegmentName){
   TRACE("trying to get Segment by Name, Name: " + aSegmentName);
   try {
       return (SegmentBase*)getSegment(_segmentsByName.at(aSegmentName));
   } catch(const std::out_of_range& oor) {
       TRACE("## Error ## The requested segment with name '" + aSegmentName + "' does not exist in map (" + std::string(oor.what()) + ")");
       throw SegmentNotExistsException(FLF);
   }
}

const Segment_T& SegmentManager::getSegmentT(const uint16_t aID) const
{
    try
    {
        return _segmentsByID.at(aID);
    }
    catch(const std::out_of_range& oor)
    {
        TRACE("## Error ## The requested segment tuple with ID '" + std::to_string(aID) + "' does not exist in map (" + std::string(oor.what()) + ")");
        throw SegmentNotExistsException(FLF);
    }
}

Segment_T& SegmentManager::getSegmentT(const uint16_t aID)
{
    return const_cast<Segment_T&>(static_cast<const SegmentManager&>(*this).getSegmentT(aID));
}

const Segment_T& SegmentManager::getSegmentT(const std::string& aName) const
{
    try
    {
        return getSegmentT(_segmentsByName.at(aName));
    }
    catch(const std::out_of_range& oor)
    {
        TRACE("## Error ## The requested segment tuple with name '" + aName + "' does not exist in map (" + std::string(oor.what()) + ")");
        throw SegmentNotExistsException(FLF);
    }
}

Segment_T& SegmentManager::getSegmentT(const std::string& aName)
{
    return const_cast<Segment_T&>(static_cast<const SegmentManager&>(*this).getSegmentT(aName));
}

void SegmentManager::storeSegments()
{
    for (auto& segmentsItr : _segments)
    {
        segmentsItr.second->storeSegment();   
    }
}

void SegmentManager::createMasterSegments(PartitionFile* aPartition, const std::string& aName)
{
    TRACE("Creation of master segments starts...");
     // create 2 Master Segments
     // Master Segment for Partitions
     SegmentFSM_SP* lPSeg = new SegmentFSM_SP(_counterSegmentID++, *aPartition, *_cb);
    _segments[lPSeg->getID()] = lPSeg;
    
    Segment_T lPSegT(aPartition->getID(),lPSeg->getID(),aName,2,lPSeg->getIndexPages().at(0));
    TRACE("ID is "+std::to_string(aPartition->getID()));
    TRACE("MasterSegPart created");
    // Master Segment for Segments
    SegmentFSM_SP* lSSeg = new SegmentFSM_SP(_counterSegmentID++, *aPartition, *_cb);
    _segments[lSSeg->getID()] = lSSeg;

    Segment_T lSSegT(aPartition->getID(), lSSeg->getID(), _masterSegSegName, 2, lSSeg->getIndexPages().at(0));
    TRACE("First Page is "+std::to_string(lSSeg->getIndexPages().at(0)));   
    TRACE("MasterSegSeg created.");
    // store the to Segments into the Master Segment of Segments
    createSegmentSub(lSSegT);
    createSegmentSub(lPSegT);
    TRACE("Creation of master segments finished");
}

string_vt SegmentManager::getSegmentNames() noexcept {
    string_vt names;
    for (const auto& elem : _segmentsByID) {
        names.push_back(elem.second.name());
    }
    return names;
}

string_vt SegmentManager::getSegmentNamesForPartition(uint8_t aPID) noexcept{
    string_vt qsnames;
    for (const auto& elem : _segmentsByID) {
        if (elem.second.partID() == aPID)
            qsnames.push_back(elem.second.name());
    }
    return qsnames;
}

// old code
/*
int SegmentManager::storeSegmentManager(PartitionBase& aPartition)
{
    // TODO: use code that can be reused, delete rest
    std::cout<<"store Segement Manager"<<std::endl;
    aPartition.open();
    // store all segments
    storeSegments();
    std::cout<<"stored segements"<<std::endl;
    // store yourself
    auto lsegmentsIterator = _segments.begin();
    // uint lsegmentsCounter = 0;
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
    // TODO: use code that can be reused, delete rest


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
        lSMH = *(segment_index_header_t *)(lPageBuffer + aPartition.getPageSize() - sizeof(segment_index_header_t)); // I guess this is what you wanted, Jonas?
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

