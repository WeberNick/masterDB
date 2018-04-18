#include "args.hh"

void construct_arg_desc(argdesc_vt& x) {
	// typedef argdesc_t<Args, char>        carg_t;
	// typedef argdesc_t<Args, int>         iarg_t;
	typedef argdesc_t<Args, uint>        uarg_t;
	// typedef argdesc_t<Args, float>       farg_t;
	// typedef argdesc_t<Args, double>      darg_t;
	typedef argdesc_t<Args, bool>        barg_t;
	typedef argdesc_t<Args, std::string> sarg_t;

	x.push_back( new barg_t("--help", false, &Args::help, "print this message" ));
	x.push_back( new barg_t("--test", false, &Args::test, "conduct a complete DBS test"));
	x.push_back( new barg_t("--install", false, &Args::install, "install the DBS on the defined master partition"));
	x.push_back( new barg_t("--trace", false, &Args::trace, "sets the flag for tracing"));
	x.push_back( new uarg_t("--segment-index-page", 1, &Args::segmentIndexPage, "the page index where to store the segment manager"));
	x.push_back( new uarg_t("--frames", 100000, &Args::bufferFrames, "the size of the buffer pool (frames = no pages managed by buffer manager)"));
	x.push_back( new sarg_t("--master", "no default", &Args::masterPartition, "path to master partition"));
    x.push_back( new sarg_t("--trace-path", "../logs/", &Args::tracePath, "path to log files"));
}

Args::Args() : 	
	_help(false), 
	_test(false),
	_install(false),
    _trace(false),
	_segmentIndexPage(1),
    _noBufFrames(100000), //100,000 Frames * 4KB Pages = 400MB Bufferpool
	_masterPartition("no default"),
    _tracePath("../logs/")
{}


