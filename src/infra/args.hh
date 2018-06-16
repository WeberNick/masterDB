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
#pragma once 

#include "argbase.hh"
#include "types.hh"
#include <cstdlib>
#include <string>


class Args {
  public:
    Args() = default;

  public:
    inline bool help() noexcept { return _help; }
    inline void help(const bool &x) noexcept { _help = x; }
    
    inline bool test() noexcept { return _test; }
    inline void test(const bool &x) noexcept { _test = x; }

    inline bool install() noexcept { return _install; }
    inline void install(const bool &x) noexcept { _install = x; }

    inline bool trace() noexcept { return _trace; }
    inline void trace(const bool &x) noexcept { _trace = x; }

    inline uint segmentIndexPage() noexcept { return _segmentIndexPage; }
    inline void segmentIndexPage(const uint &x) noexcept { _segmentIndexPage = x; }

    inline uint bufferFrames() noexcept { return _noBufFrames; }
    inline void bufferFrames(const uint &x) noexcept { _noBufFrames = x; }

    inline const std::string masterPartition() noexcept { return _masterPartition; }
    inline void masterPartition(const std::string &x) noexcept { _masterPartition = x; }
    
    inline const std::string tracePath() noexcept { return _tracePath; }
    inline void tracePath(const std::string &x) noexcept { _tracePath = x; }


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

using argdesc_vt = std::vector<argdescbase_t<Args> *>;

void construct_arg_desc(argdesc_vt &aArgDesc);
