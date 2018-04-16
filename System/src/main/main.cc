#include "infra/args.hh"
#include "infra/types.hh"
#include "infra/exception.hh"
#include "infra/trace.hh"

#include "db_instance_manager.hh"

#include "test.hh"

#include <iostream>
#include <cstdlib>

/* Pass path to partition as argument when executing!
   
   Print Overview:
   >      indicates print from main
   bold   indicates print from other methods
   normal indicates terminal output (e.g. output from creation of partition with linux command) */
int main(const int argc, const char *argv[]) {
    /* Parse Command Line Arguments */
    Args lArgs;
    argdesc_vt lArgDesc;
    construct_arg_desc(lArgDesc);

    if (!parse_args<Args>(1, argc, argv, lArgDesc, lArgs)) {
        std::cerr << "error while parsing arguments." << std::endl;
        return -1;
    }

    if (lArgs.help()) {
       print_usage(std::cout, argv[0], lArgDesc);
       return 0;
    }
    
    //Actual programm starts here.     
    try
    {

        const size_t C_PAGE_SIZE = 4096;

        const control_block_t lCB = {
            lArgs.masterPartition(), 
            lArgs.tracePath(),
            C_PAGE_SIZE,
            lArgs.bufferFrames(),
            lArgs.trace()
        };

        //init all global singletons
        Trace::getInstance().init(lCB);


        const std::string C_PATH = lArgs.path();
        const uint C_GROWTH_INDICATOR = lArgs.growthIndicator();

        InterpreterFSIP::setPageSize(C_PAGE_SIZE);

	    /* Test call in test.hh */
        if (lArgs.test()) {
            test(C_PATH, C_GROWTH_INDICATOR, lCB);
        }

	    DatabaseInstanceManager& lDBIM = DatabaseInstanceManager::getInstance(lCB);
        //boot..

        //shutdown


    
    } //Any exceptions from which there is no recover possible, are catched here 
    catch(const ReturnException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
