#include "infra/types.hh"
#include "infra/args.hh"

#include "db_instance_manager.hh"

#include "test.hh"


#include <iostream>

/* Pass path to partition as argument when executing!
   
   Print Overview:
   >      indicates print from main
   bold   indicates print from other methods
   normal indicates terminal output (e.g. output from creation of partition with linux command) */
int main(const int argc, const char *argv[]) {	
	/***************************************************************************************************
	** Parse Command Line Arguments ********************************************************************
	***************************************************************************************************/

	Args lArgs;
	argdesc_vt lArgDesc;
	construct_arg_desc(lArgDesc);

	if(!parse_args<Args>(1, argc, argv, lArgDesc, lArgs)) 
	{
		std::cerr << "error while parsing arguments." << std::endl;
		return -1;
	}

	if(lArgs.help()) 
	{
		print_usage(std::cout, argv[0], lArgDesc);
		return 0;
	}

	const std::string 	C_PATH						= lArgs.path();
	const uint 			C_PARTITION_SIZE_IN_PAGES 	= lArgs.partitionSizeInPages();
	const uint 			C_PAGE_SIZE 				= lArgs.pageSize();
	const uint 			C_SEGMENT_INDEX_PAGE 		= lArgs.segmentIndexPage();
	const uint 			C_GROWTH_INDICATOR 			= lArgs.growthIndicator();

	std::cout << "Path: " << C_PATH << std::endl;
	std::cout << "PSize: " << C_PARTITION_SIZE_IN_PAGES << std::endl;
	std::cout << "PageSize: " << C_PAGE_SIZE << std::endl;
	std::cout << "Index: " << C_SEGMENT_INDEX_PAGE << std::endl;
	std::cout << "Growth: " << C_GROWTH_INDICATOR << std::endl;

	FSIPInterpreter::setPageSize(lArgs.pageSize());

	/***************************************************************************************************
	** Test call in test.hh ****************************************************************************
	***************************************************************************************************/
	if(lArgs.test())
	{
		test(C_PATH, C_PARTITION_SIZE_IN_PAGES, C_PAGE_SIZE, C_SEGMENT_INDEX_PAGE, C_GROWTH_INDICATOR);
	}

	/***************************************************************************************************
	** Todo... Boot Sequence ***************************************************************************
	***************************************************************************************************/

	DatabaseInstanceManager::getInstance().getPartMngr();
	DatabaseInstanceManager::getInstance().getSegMngr();
	

    return 0;
}