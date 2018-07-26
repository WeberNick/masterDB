#include "parser.hh"

CP::Command::Command(const CP& aCP,
                     const char* aName,
                     const bool aHasParams,
                     const size_t aCommandLength,
                     const size_t aNumParams,
                     CP::CommandStatus (CP::*aFunc)(const char_vpt*) const,
                     const char* aMsg,
                     const char* aUsageInfo) :
    _cp(aCP),
    _name(aName),
    _hasParams(aHasParams),
    _comLength(aCommandLength),
    _numParams(aNumParams),
    _func(aFunc),
    _helpMsg(aMsg),
    _usageInfo(aUsageInfo)
{}

const char* CP::_HELP_FLAG = "-h";

CommandParser::CommandParser() :
    _commands
    {
        Command(*this, "INSTALL",              true,  1,  1,       &CP::com_install,      "Installs the database system, storing the master partition at a given path", "INSTALL [str:path]\nExample:\n  INSTALL /usr/system/MasterPartition"),
        Command(*this, "BOOT",                 true,  1,  1,       &CP::com_boot,         "Boots the database system using the master partition at a given path", "BOOT [str:path]\nExample:\n  BOOT /usr/system/MasterPartition"),
        Command(*this, "SHUTDOWN",             false, 1,  0,       &CP::com_shutdown,     "Shuts down the database system. Enables you to boot with another masterPartition without exiting", "SHUTDOWN"),
        Command(*this, "HELP",                 false, 1,  0,       &CP::com_help,         "Displays usage information.", "HELP"),
        Command(*this, "CREATE PARTITION",     true,  2,  3,       &CP::com_create_p,     "Create a partition at a given path to a partition file with a name and a growth indicator of at least 8.", "CREATE PARTITION [str:path_to_partfile] [str:name] [int:growth_indicator >= 8]"),
        Command(*this, "CREATE RAW PARTITION", true,  3,  3,       &CP::com_create_rp,    "Create a raw partition at a given path to a partition", "")
        Command(*this, "DROP PARTITION",       true,  2,  1,       &CP::com_drop_p,       "Drop a partition by name.", "DROP PARTITION [str:name]"),
        Command(*this, "CREATE SEGMENT",       true,  2,  2,       &CP::com_create_s,     "Create a segment for a given partition with a name.", "CREATE SEGMENT [str:partname] [str:segname]"),
        Command(*this, "DROP SEGMENT",         true,  2,  1,       &CP::com_drop_s,       "Drop a segment by its name. Segment names are globally unique, thus no partition has to be provided.", "DROP SEGMENT [str:segname]"),
        Command(*this, "INSERT INTO",          true,  2,  INVALID, &CP::com_insert_tuple, "Insert a tuple into a segment.", "INSERT INTO [str:segname] [str:relation] [Args...]\n\nExample: Creation of an Employee with double:salary, int:age and string:name\n  INSERT INTO segname EMPLOYEE 80000 30 Mueller"),
        Command(*this, "SHOW PARTITION",       true,  2,  1,       &CP::com_show_part,    "Show detailed information for a partition.", "SHOW PARTITION [str:partname]"),
        Command(*this, "SHOW PARTITIONS",      false, 2,  0,       &CP::com_show_parts,   "Show all partitions.", "SHOW PARTITIONS"),
        Command(*this, "SHOW SEGMENT",         true,  2,  1,       &CP::com_show_seg,     "Show detailed information for a segment.", "SHOW SEGMENT [str:segname]"),
        Command(*this, "SHOW SEGMENTS",        false, 2,  0,       &CP::com_show_segs,    "Show all segments for a given partition.", "SHOW SEGMENTS"),
        Command(*this, "EXIT",                 false, 1,  0,       &CP::com_exit,         "Shut down masterDB and exit.", "EXIT")
    },
    _maxCommandLength(0),
    _reader(),
    _cb(nullptr)
{}

void CommandParser::init(CB& aControlBlock, const char* aPrompt, const char aCommentChar) {
    if (!_cb)
    {   
        _cb = &aControlBlock;
        _reader.set_prompt(aPrompt);
        _reader.set_commentchar(aCommentChar);
        for (size_t i = 0; i < _commands.size(); ++i)
        {
            _maxCommandLength = strlen(_commands[i]._name) > _maxCommandLength ? strlen(_commands[i]._name) : _maxCommandLength;
        }
        runcli();
    }
}

void CommandParser::runcli()
{
    printw();
    for (_reader.open(); _reader.ok(); _reader.next())
    {
        _reader.split_line(' ', true);
        const char_vpt& splits = _reader.splits();
        const Command* com = findCommand(&splits);
        if (com != NULL)
        {
            std::string comname(com->_name);
            if (!DatabaseInstanceManager::getInstance().isRunning() && !(comname == "INSTALL" || comname == "BOOT")
                && comname != "HELP" && comname != "EXIT")
            {
                std::cout << "The database system is not running yet." << "\n"
                          << "If you have already installed it, you can boot the system by providing a path to the master partition." << "\n"
                          << "If you have not yet installed the database system, you can install it by " << "\n"
                          << "  providing a valid path to a directory where you want to store the master partition." << "\n"
                          << "Type \"INSTALL -h\" or \"BOOT -h\" for more information." << "\n" << std::endl;
                continue;
            }
            else if (DatabaseInstanceManager::getInstance().isRunning() && (comname == "INSTALL" || comname == "BOOT"))
            {
                std::cout << "The database system is already running @masterPartition: \"" << _cb->mstrPart() << "\")" << "\n" << std::endl;
                continue;
            }
            else
            {
                if (splits.size() == com->_comLength + 1 && *splits[splits.size() - 1] == *CP::_HELP_FLAG)
                {
                    std::cout << "Help information for command: " << com->_name << "\n"
                              << "Purpose:\n  " << com->_helpMsg << "\n"
                              << "Usage:\n  " << com->_usageInfo << "\n"
                              << std::endl;
                }
                else if ((splits.size() - com->_comLength) != com->_numParams && !(com->_numParams == INVALID))
                {
                    std::cout << "Wrong number of args.\n"
                              << "Usage:\n  " << com->_usageInfo << "\n"
                              << std::endl;
                }
                else
                {
                    CP::CommandStatus rec;
                    if (com->_hasParams)
                    {
                        const char_vpt args(&splits[com->_comLength], &splits[splits.size()]);
                        auto future = Pool::Default::submitJob(com->_func, this, &args);
                        rec = (CP::CommandStatus) future.get();
                    }
                    else
                    {
                        auto future = Pool::Default::submitJob(com->_func, this, nullptr);
                        rec = (CP::CommandStatus) future.get();
                    }
                    if (rec == CP::CommandStatus::EXIT || rec == CP::CommandStatus::UNKNOWN_ERROR)
                    {
                        break;
                    }
                    else if (rec == CP::CommandStatus::WRONG)
                    {
                        std::cout << "Wrong type or value of some argument.\n"
                                  << "Usage - " << com->_usageInfo << "\n"
                                  << std::endl;
                    }
                    else if (rec == CP::CommandStatus::CONTINUE)
                    {
                        continue;
                    }
                }
            }
        }
        else
        {
            std::cout << "Invalid command, type:\n"
                      << "  'HELP'       for a list of commands or\n"
                      << "  [COMMAND] " << _HELP_FLAG << " for information on a single command.\n"
                      << std::endl;
        }
    }
    DatabaseInstanceManager::getInstance().shutdown(); // only in case of error abortion
}

const CP::Command* CommandParser::findCommand(const char_vpt* splits)
{
    std::string com(splits->at(0));
    std::string com_warg = "";
    if (splits->size() > 1)
    {
        com_warg = std::string(splits->at(0)) + " " + std::string(splits->at(1));
    }
    std::transform(com.begin(), com.end(), com.begin(), ::toupper);
    std::transform(com_warg.begin(), com_warg.end(), com_warg.begin(), ::toupper);
    for (size_t i = 0; i < _commands.size() ; ++i)
    {
        std::string name(_commands[i]._name);
        if (name == com || name == com_warg) 
        {
            return &_commands[i];
        }
    }
    return nullptr;
}

std::string CommandParser::findCommand(const std::string& arg) const
{
    for (size_t i = 0; i < _commands.size(); ++i)
    {
        std::string name(_commands[i]._name);
        if (name == arg)
            return name;
    }
    return "";
}

CP::CommandStatus CP::com_install(const char_vpt* args) const
{
    std::string path(args->at(0));
    if(!FileUtil::hasValidDir(path))
    {
        std::cout << "The given path is invalid. Cannot create a master partition at this path.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    try
    {
        _cb->_install = true; // install
        _cb->_masterPartition = path;
        if(DatabaseInstanceManager::getInstance().isInit())
        {
            DatabaseInstanceManager::getInstance().install();
        }
        else
        {
            DatabaseInstanceManager::getInstance().init(*_cb); // installs the DBS
        }
        std::cout << "Installed the datbase system successfully at " << path << "." << "\n" << std::endl;
    }
    catch(const PartitionExistsException& pex)
    {
        std::cout << "A (Master-)Partition already exists at this location. Please try again by providing an alternative path or consider booting the system." << "\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e) 
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_boot(const char_vpt* args) const
{
    std::string path(args->at(0));
    if(!(FileUtil::exists(path)))
    {
        std::cout << "The given path to the masterPartition is invalid." << "\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    try
    {
        _cb->_install = false; // boot
        _cb->_masterPartition = path;
        if(DatabaseInstanceManager::getInstance().isInit())
        {
            DatabaseInstanceManager::getInstance().boot();
        }
        else
        {
            DatabaseInstanceManager::getInstance().init(*_cb); // boots the DBS
        }
        std::cout << "Booted the datbase system successfully from \"" << path << "\"." << "\n" << std::endl;
    }
    catch(const PartitionNotExistsException& pex)
    {
        std::cout << "No MasterPartition exists at this path. Consider installing the database system." << "\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e) 
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_shutdown(const char_vpt* args) const
{
    DatabaseInstanceManager::getInstance().shutdown();
    std::cout << "Successfully shutdown database running @masterPartition: \"" << _cb->mstrPart() << "\"." << "\n" << std::endl;
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_help(const char_vpt* args) const
{
    printh();
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_exit(const char_vpt* args) const
{
    if (DatabaseInstanceManager::getInstance().isRunning())
    {
        DatabaseInstanceManager::getInstance().shutdown();
        std::cout << "System was shut down successfully. ";
    }
    printe();
    return CP::CommandStatus::EXIT;
}

CP::CommandStatus CP::com_create_p(const char_vpt* args) const
{
    std::string path(args->at(0));
    std::string partName(args->at(1));
    uint growthInd;
    if (!_reader.isnumber(args->at(2)))
        return CP::CommandStatus::WRONG;
    else growthInd = atoi(args->at(2));
    try
    {
        const bool created = PartitionManager::getInstance().createPartitionFileInstance(path, partName, growthInd).second;
        if (created)
        {
            std::cout << "Successfully created Partition \"" << partName << "\" at \"" << args->at(0) << "\".\n" << std::endl;
        } 
        else
        {
            std::cout << "PartitionFile at \"" << args->at(0) << "\" already exists, \"" << partName << "\" could not be created.\n" << std::endl;
        }
    }
    catch(const PartitionExistsException& pex) 
    {
         const std::string& partLoc = PartitionManager::getInstance().getPathForPartition(partName);
         std::cout << "Partition \"" << partName << "\" already exists at \"" << partLoc << "\".\n" << std::endl;
         return CP::CommandStatus::CONTINUE;
    }
    catch(const InvalidArgumentException& iaex) 
    {
        std::cout << "Invalid argument was provided:" << std::endl;
        std::cout << iaex.what() << "\n" << std::endl;
        return CP::CommandStatus::CONTINUE; // return OK instead of WRONG because iaex.what() already displays information
    }
    catch(const InvalidPathException& ipex) 
    {
        std::cout << "The provided path is invalid:" << std::endl;
        std::cout << ipex.what() << "\n" << std::endl;
        return CP::CommandStatus::CONTINUE; // return OK instead of WRONG because iaex.what() already displays information
    }
    catch(const PartitionFullException& ex) 
    {
        std::cout << "Partition Full.\n" << std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    catch(const fs::filesystem_error& fse) 
    {
        std::cout << "Filesystem Exception.\n" << std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    catch(const std::exception& e) 
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_drop_p(const char_vpt* args) const
{
    std::string partName(args->at(0));    
    try
    {
        PartitionManager::getInstance().deletePartition(partName);
        std::cout << "Partition \"" << partName << "\" was deleted.\n" << std::endl; 
    }
    catch(const PartitionNotExistsException& pnee)
    {
        std::cout << "Partition \"" << partName << "\" does not exist. Nothing was dropped.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_create_s(const char_vpt* args) const
{
    const std::string partName(args->at(0));
    const std::string segName(args->at(1));
    try
    {
        SegmentFSM_SP* lSegment = SegmentManager::getInstance().createNewSegmentFSM_SP(*PartitionManager::getInstance().getPartition(partName), segName);
        if (lSegment)
        {
            std::cout << "Successfully created Segment \"" << segName << "\" for Partition \"" << partName << "\".\n" << std::endl;
        }
        else
        {
            std::cout << "Segment \"" << segName << "\" for Partition \"" << partName << "\" could not be created.\n" << std::endl;
        }
    }
    catch(const SegmentExistsException& see)
    {
        std::cout << "Segment \"" << segName << "\" already exists for Partition \"" << partName << "\".\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const PartitionNotExistsException& pnee)
    {
        std::cout << "Partition \"" << partName << "\" does not exist. Segment \"" << segName << "\" could not be created.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_drop_s(const char_vpt* args) const
{
    std::string segName(args->at(0));
    try
    {
        SegmentManager::getInstance().deleteSegment(segName);
        std::cout << "Segment \"" << segName << "\" was deleted.\n" << std::endl; 
    }
    catch(const SegmentNotExistsException& snee)
    {
        std::cout << "Segment \"" << segName << "\" does not exist. Nothing was dropped.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_insert_tuple(const char_vpt* args) const
{
    /* INSERT INTO Seg_Emp Employee 80000 30 Mueller */
    TRACE("Start to insert Tuple");
    std::string segName(args->at(0));
    std::string type(args->at(1));
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);
    try
    {
        if (type == "EMPLOYEE")
        {
            if (args->size() != (2 + 3)) // hard coded because we only consider one relation: Employee
            { 
                return CP::CommandStatus::WRONG;
            }
            else
            {
                double emp_sal = atof(args->at(2));
                int emp_age = atoi(args->at(3));
                std::string emp_name(args->at(4));
                Employee_T e(emp_name, emp_sal, emp_age);
                TRACE("INSERT TUPLE EMPLOYEE");
                ((SegmentFSM_SP*)(SegmentManager::getInstance().getSegment(segName)))->insertTuple(e);
                std::cout << "Tuple insertion into " << type << " succeeded.\n" << std::endl;
            }
        }
        else
        {
            std::cout << "Relation " << type << " is not supported.\n" << std::endl;
            return CP::CommandStatus::CONTINUE;
        }
    }
    catch(const SegmentNotExistsException& snee)
    {
        std::cout << "Segment \"" << segName << "\" does not exist. Could not insert Tuple.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_show_part(const char_vpt* args) const
{
    std::string partName(args->at(0));
    try
    {
        const Partition_T& part = PartitionManager::getInstance().getPartitionT(partName);
        size_t partID = part.ID();
        const string_vt& segNames = SegmentManager::getInstance().getSegmentNamesForPartition(partID);
        std::vector<Segment_T> partSegs;
        partSegs.reserve(segNames.size());
        for (const auto& name : segNames)
        {
            const Segment_T& seg_t = SegmentManager::getInstance().getSegmentT(name);
            partSegs.push_back(seg_t);
        }
        std::cout << "PartitionID:    " << partID << std::endl;
        std::cout << "Partition:      " << part.name() << std::endl;
        // std::cout << "Partition Type: " << part.type() << std::endl;
        std::cout << std::endl;
        std::cout << "Segments:       ";
        if (segNames.size() == 0)
        {
            std::cout << "No Segments exist for Partition \"" << partName << "\".\n" << std::endl;
        }
        else
        {
            std::cout << std::endl;
            pprintelems(partSegs);
        }
    }
    catch(const PartitionNotExistsException& oore)
    {
        std::cout << "Partition \"" << partName << "\" does not exist.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_show_parts(const char_vpt* args) const
{
    try
    {
        const std::string& mspname = PartitionManager::getInstance().getMasterSegPartName();
        SegmentFSM_SP* msegFSM = (SegmentFSM_SP*)(SegmentManager::getInstance().getSegment(mspname));
        const std::vector<Partition_T>& parts = msegFSM->getTuples<Partition_T>(msegFSM->scan());
        pprintelems(parts);
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_show_seg(const char_vpt* args) const
{
    std::string segName(args->at(0));
    try
    {
        const Segment_T& seg = SegmentManager::getInstance().getSegmentT(segName);
        std::cout << "SegmentID: " << seg.ID() << std::endl;
        // std::cout << "Segment:      " << seg.name() << std::endl;
        std::cout << "Partition: " << PartitionManager::getInstance().getPartitionName(seg.partID()) << std::endl;
        // std::cout << "PartitionID:  " << seg.partID() << std::endl;
        // std::cout << "Segment Type: " << seg.type() << std::endl;
        std::cout << std::endl;

        SegmentFSM_SP* segFSM = (SegmentFSM_SP*)(SegmentManager::getInstance().getSegment(segName));
        const std::vector<Employee_T>& tups = segFSM->getTuples<Employee_T>(segFSM->scan());
        pprintelems(tups);
    }
    catch(const SegmentNotExistsException& oore)
    {
        std::cout << "Segment \"" << segName << "\" does not exist.\n" << std::endl;
        return CP::CommandStatus::CONTINUE;
    }
    catch(const std::exception& e)
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

CP::CommandStatus CP::com_show_segs(const char_vpt* args) const
{
    try
    {
        const std::string& mssname = SegmentManager::getInstance().getMasterSegSegName();
        SegmentFSM_SP* ssegFSM = (SegmentFSM_SP*)(SegmentManager::getInstance().getSegment(mssname));
        const std::vector<Segment_T>& segs = ssegFSM->getTuples<Segment_T>(ssegFSM->scan());
        pprintelems(segs);
    }
    catch(const std::exception& e) 
    {
        std::cout << "An error occurred: " << e.what() << "\nAbort.\n"<< std::endl;
        return CP::CommandStatus::UNKNOWN_ERROR;
    }
    return CP::CommandStatus::CONTINUE;
}

void CommandParser::printw() const
{
    std::cout << "Welcome to the command line interface of masterDB.\n\n"
              << "Type 'HELP' for help and usage information.\n"
              << "Type 'EXIT' for shutting down the database.\n"
              << "Type [COMMAND] " << _HELP_FLAG << " for further information on a single command.\n" << std::endl;
}

void CommandParser::printh() const
{
    std::cout << "List of all masterDB commands:" << std::endl;
    for (size_t i = 0; i < _commands.size() ; ++i)
    {
        std::cout << "  " << std::setw(_maxCommandLength + 1) << std::left << _commands[i]._name << " " << _commands[i]._helpMsg << std::endl;
    }
    std::cout << "\n" << "For further information on a single command, type:\n"
                      << "  [COMMAND] " << _HELP_FLAG << "\n" << std::endl;
}

void CommandParser::printe() const
{
    std::cout << "Good Bye.\n" << std::endl;
}

template <typename T>
void CommandParser::pprintelems(const std::vector<T>& tuples) const
{
    const string_vt& attrs = T::attributes();

    std::vector<uint8_t> spaces;
    spaces.reserve(attrs.size());
    for (const auto& a : attrs)
    {
        spaces.push_back(a.size()); // default init spaces with attribute length
    }
    for (const auto& tuple : tuples)
    {
        const string_vt& values = tuple.values();
        for (size_t i = 0; i < values.size(); ++i)
        {
            auto& value = values.at(i);
            if (value.size() > spaces.at(i))
            {
                spaces.at(i) = value.size(); 
            }
        }
    }

    printptable(spaces);
    pprintelem(attrs, spaces);
    printptable(spaces);
    for (const auto& tuple : tuples)
    {
        pprintelem(tuple.values(), spaces);
    }
    printptable(spaces);
    std::cout << std::endl;
} 

void CommandParser::pprintelem(const string_vt& values, const std::vector<uint8_t>& spaces) const
{
    std::string sep = "|";
    for (size_t i = 0; i < values.size(); ++i)
    {
        std::cout << sep << " ";
        sep = "";
        const std::string& line = values.at(i);
        std::cout << line;
        for (size_t j = 0; j < spaces.at(i) - line.size(); ++j)
            std::cout << " ";
        std::cout << " |";
    }
    std::cout << std::endl;
}

void CommandParser::printptable(const std::vector<uint8_t>& spaces) const
{
    std::cout << "+";
    for (auto& s : spaces)
    {
        for (uint8_t i = 0; i < s + 2; ++i)
            std::cout << "-";
        std::cout << "+";
    }
    std::cout << std::endl;
}

void CommandParser::multiexec(const string_vt& commands)
{
    for (const auto& line : commands)
    {
        char* cstr = new char[line.length() + 1];
        std::strcpy(cstr, line.c_str());
        _reader.setNonCommentLine(cstr);
        _reader.split_line(' ', true);
        const char_vpt& splits = _reader.splits();
        const Command* com = findCommand(&splits);
        if (com != NULL)
        {
            if ((splits.size() - com->_comLength) != com->_numParams && !(com->_numParams == INVALID))
            {
                std::cout << "Wrong number of args.\n"
                          << "Usage:\n  " << com->_usageInfo << "\n"
                          << std::endl;
            }
            else
            {
                CP::CommandStatus rec;
                if (com->_hasParams)
                {
                    const char_vpt args(&splits[com->_comLength], &splits[splits.size()]);
                    auto future = Pool::Default::submitJob(com->_func, this, &args);
                    rec = (CP::CommandStatus) future.get();
                }
                else
                {
                    auto future = Pool::Default::submitJob(com->_func, this, nullptr);
                    rec = (CP::CommandStatus) future.get();
                }
                if (rec == CP::CommandStatus::EXIT || rec == CP::CommandStatus::UNKNOWN_ERROR)
                {
                    delete[] cstr;
                    break;
                }
            }
        }
        else
            std::cout << "Invalid command" << std::endl;
        delete[] cstr;
    }
    DatabaseInstanceManager::getInstance().shutdown();
}
