/**
 *  @file   db_instance_manager.hh
 *  @author Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *  @brief  Singleton class managing everything needed for the DB instance to boot
 *  @bugs   Currently no bugs known
 *  @todos  Currently no todos
 *
 *  @section TBD
 */

#ifndef DB_INSTANCE_MANAGER_HH
#define DB_INSTANCE_MANAGER_HH

#include "infra/types.hh"
#include "partition/partition_manager.hh"
#include "segment/segment_manager.hh"

#include <string>

const std::string C_PATH_TO_MASTER_PARTITION = ""; //Location may change (member, command line arg,...)

class DatabaseInstanceManager {
	private:
		explicit DatabaseInstanceManager(const std::string aPathToMasterPartition);
		DatabaseInstanceManager(const DatabaseInstanceManager& aDatabaseInstanceManager) = delete;
		DatabaseInstanceManager &operator=(const DatabaseInstanceManager &aDatabaseInstanceManager) = delete;
		~DatabaseInstanceManager();

	public:
		/**
		 *  @brief  This function is the only way to get access to the PartitionManager instance
		 *
		 *  @return reference to the only PartitionManager instance
		 */
		static DatabaseInstanceManager& getInstance(const std::string aPathToMasterPartition) {
			static DatabaseInstanceManager lDBIM_Instance(aPathToMasterPartition);
			return lDBIM_Instance;
		}

	public:
		void install();
		void boot();
		void shutdown();

	public:
		inline PartitionManager& getPartMngr() { return _partMngr; }
		inline SegmentManager& getSegMngr() { return _segMngr; }

	private:
		void loadPartitionManager(); //called in boot, loads the PartMngr from the master part
		void loadSegmentManager(); //called in boot, loads the SegMngr from the master part

	private:
                PartitionFile _masterPartition;
		PartitionManager& _partMngr;
		SegmentManager& _segMngr;
                uint _partIndex; //Index of first segment storing pages with partition tuples, should be 1
                uint _segIndex; //Index of first segment storing pages with segment tuples, should be 2
};

#endif
