#include "../infra/args.hh"
#include "../infra/types.hh"
#include "../infra/exception.hh"
#include "../infra/trace.hh"

#include "../infra/tuples.hh"

#include "db_instance_manager.hh"
#include "../cli/parser.hh"

#include <iostream>
#include <cstdlib>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void test(const control_block_t &aControlBlock) {
    // std::cout << "\n" << aControlBlock._masterPartition <<   std::endl;

    // PartitionFile *lPartFile =
    // PartitionManager::getInstance().createPartitionFileInstance("$HOME/Partition",
    // "DefaultName", 1000);
    //	std::cout << "## TEST: Size in Pages (should be 0): " <<
    //lPartFile->getSizeInPages() << std::endl;

    Trace::getInstance().log(FLF, "Trace works");
    std::string lHome(std::getenv("HOME"));
    std::string lPath = lHome + std::string("/MasterTeamProjekt/Partition");
    std::cout << "Path: " << lPath << std::endl;
    //PartitionFile* lFile = PartitionManager::getInstance().createPartitionFileInstance(lPath, "MyPartition", 100); 
  //  if(lFile==nullptr) std::cout<<"fail"<<std::endl;
  //  SegmentManager::getInstance().createNewSegmentFSM(*lFile,"blub");
    
    //a test scenario

    //install    
    DatabaseInstanceManager::getInstance().init(aControlBlock);
    
    TRACE("create a new Partition");
    PartitionFile*  lPart = PartitionManager::getInstance().createPartitionFileInstance(lPath,"blub",100);
    TRACE("create a Segment");
    SegmentManager::getInstance().createNewSegmentFSM_SP(*lPart,"bla");
    char lTuple[] = "ThisDoesNotWork";
    TRACE("get segment and insert tuple");
    SegmentFSM_SP* lSeg = (SegmentFSM_SP*) SegmentManager::getInstance().getSegment("bla");
    //insert a tuple
    lSeg->insertTuple((byte*) &lTuple,sizeof(lTuple));
   // char lTuple2[] = "SomeMoreRandomChars";
   // lSeg->insertTuple((byte*) &lTuple2,20);
   TRACE("getMasterSegment");
    lSeg = (SegmentFSM_SP*) SegmentManager::getInstance().getSegment(1);
    lSeg->getPage(0,kSHARED);
    lSeg->printPageToFile(0,false);

   // TRACE(" FLUSH PAGE TO DISK");
    //BufferManager::getInstance().flushAll();
  //  TRACE(" TRY TO LOAD TUPLE AGAIN");
  //  lSeg->getPage(0,kSHARED);
  //  lSeg->printPageToFile(0,true);
    DatabaseInstanceManager::getInstance().shutdown();
    TRACE("SHUTDOWN COMPLETED");
}
void testStartUp(const control_block_t &aControlBlock){
    
    DatabaseInstanceManager::getInstance().init(aControlBlock);
    TRACE("GET SEGMENT");
    SegmentFSM_SP* lSeg = (SegmentFSM_SP*) SegmentManager::getInstance().getSegment("bla");
    TRACE("GET PAGE");
    lSeg->getPage(0,kSHARED);
    lSeg->printPageToFile(0,false);
    DatabaseInstanceManager::getInstance().shutdown();
    TRACE("SHUTDOWN COMPLETED");
}

void testTupleToDIsk()
{

    Partition_T lPart(64, "My Partition", "/path/to/partition/name", 1, 100);
    const size_t size = lPart.size();
    byte* buf = new byte[4096];
    lPart.toDisk(buf);
    char* t = (char*)buf;
    for(size_t i = 0; i < size; ++i)
    {
        std::cout << "ToChar: " << *t << std::endl;
        ++t;
    }
    Partition_T lPart2;
    lPart2.toMemory(buf);
    std::cout << lPart << std::endl;
    std::cout << lPart2 << std::endl;
}


/***********************************************************************
*  todo: test install, boot, (shutdown), buf manager, and everything  *
***********************************************************************/

/* Pass path to partition as argument when executing!
   
   Print Overview:
   >      indicates print from main
   bold   indicates print from other methods
   normal indicates terminal output (e.g. output from creation of partition with linux command) */
int main(const int argc, const char* argv[]) {
    /* Parse Command Line Arguments */
    Args lArgs;
    argdesc_vt lArgDesc;
    construct_arg_desc(lArgDesc);

    if (!parse_args<Args>(1, argc, argv, lArgDesc, lArgs)) {
        std::cerr << "Error while parsing arguments." << std::endl;
        return -1;
    } 

    if (lArgs.help() || argc == 0) {
       print_usage(std::cout, argv[0], lArgDesc);
       return 0;
    }
 /*   if(!(fs::exists(lArgs.masterPartition())))
    {
        std::cerr << "Given path to the master partition is invalid." << std::endl;
        //return -1; //wait until boot and so on works and uncomment this
    }
    if(lArgs.trace() && !fs::exists(lArgs.tracePath()))
    {
        std::cerr << "The path where to store the trace file is invalid." << std::endl;
        return -1;
    }
*/

    // DONT CHANGE THESE
    //const bool          C_INSTALL                   = lArgs.install();
    //const std::string   C_MASTER_PARTITION_PATH     = lArgs.masterPartition();
    //const std::string   C_TRACE_DIR_PATH            = lArgs.tracePath();
    //const size_t        C_PAGE_SIZE                 = 4096;
    //const size_t        C_BUFFER_POOL_SIZE          = lArgs.bufferFrames();
    //const bool          C_TRACE_ACTIVATED           = lArgs.trace();

    // Actual programm starts here.     
    try
    {
        // ASSIGN APPROPRIATE TESTING PARAS
        const bool          C_INSTALL                   = true;
        const std::string   C_MASTER_PARTITION_PATH     = std::string(std::getenv("HOME")) + std::string("/MasterTeamProjekt/MasterPartition");
        const std::string   C_TRACE_DIR_PATH            = std::string(std::getenv("HOME")) + std::string("/MasterTeamProjekt/");
        const size_t        C_PAGE_SIZE                 = 4096;
        const size_t        C_BUFFER_POOL_SIZE          = lArgs.bufferFrames();
        const bool          C_TRACE_ACTIVATED           = true;

        const control_block_t lCB = {
            C_INSTALL,
            C_MASTER_PARTITION_PATH,
            C_TRACE_DIR_PATH,
            C_PAGE_SIZE,
            C_BUFFER_POOL_SIZE,
            C_TRACE_ACTIVATED
        };
        std::cout << lCB;


       // testTupleToDIsk();

//second CB for start up.
        const control_block_t lCB2 = {
            false,
            C_MASTER_PARTITION_PATH,
            C_TRACE_DIR_PATH,
            C_PAGE_SIZE,
            C_BUFFER_POOL_SIZE,
            C_TRACE_ACTIVATED
        };
      //  CommandParser::getInstance().init(lCB);
        // init all global singletons
        Trace::getInstance().init(lCB);
        PartitionManager::getInstance().init(lCB);
        SegmentManager::getInstance().init(lCB);
        BufferManager::getInstance().init(lCB);
        //DatabaseInstanceManager::getInstance().init(lCB); // installs or boots the DBS

        

       // test(lCB);
        testStartUp(lCB2);
        // testStartUp(lCB2);
	    // Test call in test.hh
     /*   if (lArgs.test()) {
            // start thread for cli
            std::cout << "test." << std::endl;
            test(lCB);
            return EXIT_SUCCESS;
        }*/

       // CommandParser::getInstance().init(lCB, "mdb > ", '#');
    } catch(const ReturnException& ex) {
         // Any exceptions from which there is no recover possible, are catched here 
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
