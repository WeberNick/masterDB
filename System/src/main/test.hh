#include "infra/types.hh"
#include "partition/partition_file.hh"
#include "partition/partition_manager.hh"
#include "segment/segment.hh"
#include "segment/segment_manager.hh"
#include "segment/segment_fsm.hh"


void test(const std::string aPath, const uint aGrwothIndicator, const control_block_t& aControlBlock) {
    std::cout << "\n" << std::endl;

    PartitionFile *lPartFile = PartitionManager::getInstance().createPartitionFileInstance(aPath, "DefaultName", aGrwothIndicator, aControlBlock);
	std::cout << "## TEST: Size in Pages (should be 0): " << lPartFile->getSizeInPages() << std::endl;

	const uint lNumbPages = 1000000;

    if(lPartFile->create(lNumbPages) == -1){
        std::cout << "## TEST: file was not created" << std::endl;
    }

    std::cout << "## TEST: Size in Pages (should be " << lNumbPages << " ): " << lPartFile->getSizeInPages() << std::endl;

    return;
    
    PartitionManager::getInstance().getPartition(lPartFile->getID())->open();

    uint lNoFreePage = PartitionManager::getInstance().getPartition(lPartFile->getID())->allocPage();
    std::cout << "## TEST: get free Page results in Page " <<	lNoFreePage << std::endl;
    uint partID = lPartFile->getID();
    uint pageSize = PartitionManager::getInstance().getPartition(lPartFile->getID())->getPageSize();
    uint managedPages = (PartitionManager::getInstance().getPartition(lPartFile->getID())->getPageSize() - sizeof(fsm_header_t)) * 8 / 4;
    std::cout << "## TEST: pageSize " <<	pageSize << std::endl;
    std::cout << "## TEST: managed " <<	managedPages << std::endl;
    std::cout << "## TEST: partID " <<	partID << std::endl;
    InterpreterFSM fsmp;
    byte *lPagePointer = new byte[pageSize];
    
    fsmp.setPageSize(pageSize);
    fsmp.initNewFSM(lPagePointer, 3, lNoFreePage, partID, managedPages);
    PartitionManager::getInstance().getPartition(lPartFile->getID())->writePage(lPagePointer, lNoFreePage, pageSize);
    fsmp.detach();
    PartitionManager::getInstance().getPartition(lPartFile->getID())->readPage(lPagePointer, lNoFreePage, pageSize);
    fsmp.attach(lPagePointer);
    
    uint lPageSizeInBytes = pageSize - sizeof(fsm_header_t);
    std::cout << "## TEST: in Bytes " <<	lPageSizeInBytes << std::endl;

   // byte *l2PagePointer = new byte[pageSize];
   // fsmp.detach();
    //fsmp.attach(l2PagePointer);
    //PageStatus lPageStatus = fsmp.calcPageStatus(lPageSizeInBytes,  5000);
    PageStatus lPageStatus = fsmp.getPageStatus(1);

    std::cout << "## TEST: status: " <<	static_cast<int>(lPageStatus) << std::endl;

    fsmp.changePageStatus(1,static_cast<PageStatus>(8));

    

     lPageStatus = fsmp.getPageStatus(1);

    std::cout << "## TEST: status2: " <<	static_cast<int>(lPageStatus) << std::endl;

    int lIndex = fsmp.getFreePage(lPageStatus);

    std::cout << "## TEST: index: " <<	lIndex << std::endl;

    lPartFile->printPage(lNoFreePage);
    lPartFile->printPage(0);
}
