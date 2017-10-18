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

class DatabaseInstanceManager
{    
    private:
        explicit DatabaseInstanceManager();
        DatabaseInstanceManager(const DatabaseInstanceManager& aDatabaseInstanceManager) = delete;
        DatabaseInstanceManager& operator=(const DatabaseInstanceManager& aDatabaseInstanceManager) = delete;
        ~DatabaseInstanceManager();

    public:
        /**
         *  @brief  This function is the only way to get access to the PartitionManager instance
         *
         *  @return reference to the only PartitionManager instance
         */
        static DatabaseInstanceManager& getInstance()
        {
            static DatabaseInstanceManager lDBIM_Instance;
            return lDBIM_Instance;
        }

    public:
        inline PartitionManager& getPartMngr(){ return _partMngr;}
        inline SegmentManager& getSegMngr(){ return _segMngr;}
  
    private:
        PartitionManager& _partMngr;
        SegmentManager& _segMngr;
};

#endif