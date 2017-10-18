#include "args.hh"

void construct_arg_desc(argdesc_vt& x) 
{
	// typedef argdesc_t<Args, char>        carg_t;
	// typedef argdesc_t<Args, int>         iarg_t;
	typedef argdesc_t<Args, uint>        uarg_t;
	// typedef argdesc_t<Args, float>       farg_t;
	// typedef argdesc_t<Args, double>      darg_t;
	typedef argdesc_t<Args, bool>        barg_t;
	typedef argdesc_t<Args, std::string> sarg_t;

	x.push_back( new barg_t("--help", false, &Args::help, "print this message" ));
	x.push_back( new barg_t("--nsm", false, &Args::nsm, "use the n-ary storage model"));
	x.push_back( new barg_t("--pax", false, &Args::pax, "use the pax storage model"));
	// x.push_back( new uarg_t("--core", 0, &Args::core, "core number to run the execution on"));
	x.push_back( new uarg_t("--pagesize", 4096, &Args::pageSize, "memory page size to use (e.g., 4096, 8192, 16384, ...)"));
	x.push_back( new uarg_t("--alignment", 4096, &Args::alignment, "memory page alignment to use. must be a multiple of sizeof(void*), that is also a power of two"));
	x.push_back( new barg_t("--test", false, &Args::test, "conduct a complete DBS test"));
	x.push_back( new barg_t("--print", false, &Args::print, "sets the flag to print all test results into a file"));
	x.push_back( new uarg_t("--runs", 1, &Args::runs, "the number how often a test is repeated"));
	x.push_back( new sarg_t("--path", "$HOME/Partition", &Args::path, "path to partition"));
	x.push_back( new uarg_t("--segmentindexpage", 1, &Args::segmentIndexPage, "the page index where to store the segment manager"));
	x.push_back( new uarg_t("--partitionsize", 80000, &Args::partitionSizeInPages, "size of the file partition in pages"));
	x.push_back( new uarg_t("--growth", 1000, &Args::growthIndicator, "number of pages a file growths if it is full"));
}

Args::Args() : 	
	_help(false), 
	_nsm(false), 
	_pax(false),
	_test(false),
	_print(false),
	_core(0),
	_pageSize(4096),
	_alignment(4096),
	_runs(1),
	_path(std::string(getenv("HOME")).append("/Partition")),
	_segmentIndexPage(1),
	_partitionSize(80000),
	_growthIndicator(1000)
{}


