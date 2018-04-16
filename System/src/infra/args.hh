/**
 *	@file 	args.hh
 *	@author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *	@brief	Implementation of command line arguements
 *	@bugs 	Currently no bugs known
 *	@todos	Currently no todos
 *
 *	@section DESCRIPTION
 *	This class implements the command line arguements. A command line arguement has the form:
 * 	--[command] [optional parameter]
 * 	Where '--' indicates a command will follow,
 *	'command' is the command name (e.g., 'pagesize'),
 *	'optional parameter' is an optional parameter only needed for certain commands (e.g., for 'pagesize' an integer
 *is needed)
 */
 
#ifndef INFRA_ARGS_HH
#define INFRA_ARGS_HH

#include "argbase.hh"
#include "types.hh"
#include <cstdlib>
#include <string>


class Args {
  public:
    Args();

  public:
    inline const bool help() { return _help; }
    inline void help(const bool &x) { _help = x; }
    
    inline const bool test() { return _test; }
    inline void test(const bool &x) { _test = x; }

    inline const bool install() { return _install; }
    inline void install(const bool &x) { _install = x; }

    inline const bool trace() { return _trace; }
    inline void trace(const bool &x) { _trace = x; }

    inline const uint segmentIndexPage() { return _segmentIndexPage; }
    inline void segmentIndexPage(const uint &x) { _segmentIndexPage = x; }

    inline const uint bufferFrames() { return _noBufFrames; }
    inline void bufferFrames(const uint &x) { _noBufFrames = x; }

    inline const uint partitionSizeInPages() { return _partitionSize; }
    inline void partitionSizeInPages(const uint &x) { _partitionSize = x; }

    inline const uint growthIndicator() { return _growthIndicator; }
    inline void growthIndicator(const uint &x) { _growthIndicator = x; }

    inline const std::string masterPartition() { return _masterPartition; }
    inline void masterPartition(const std::string &x) { _masterPartition = x; }
    
    inline const std::string tracePath() { return _tracePath; }
    inline void path(const std::string &x) { _tracePath = x; }


  private:
    bool _help;
    bool _test;
	bool _install;
    bool _trace;
    uint _segmentIndexPage;
    uint _noBufFrames;
	std::string _masterPartition;
    std::string _tracePath;
};

typedef std::vector<argdescbase_t<Args> *> argdesc_vt;

void construct_arg_desc(argdesc_vt &aArgDesc);

#endif
