#include "parser.hh"

CP::Command::Command(const CP& aCP,
                     const char* aName,
                     const bool aHasParams,
                     const size_t aCommandLength,
                     const size_t aNumParams,
                     int (CP::*aFunc)(const char_vpt*) const,
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
    _commands{
        Command(*this, "HELP",             false, 1,  0,       &CP::com_help,         "Displays usage information.", "HELP"),
        Command(*this, "CREATE PARTITION", true,  2,  3,       &CP::com_create_p,     "Create a partition at a given destination path with a name and a growth indicator of at least 8.", "CREATE PARTITION [str:path] [str:name] [int:growth_indicator >= 8]"),
        Command(*this, "DROP PARTITION",   true,  2,  1,       &CP::com_drop_p,       "Drop a partition by name.", "DROP PARTITION [str:name]"),
        Command(*this, "CREATE SEGMENT",   true,  2,  2,       &CP::com_create_s,     "Create a segment for a given partition with a name.", "CREATE SEGMENT [str:partname] [str:segname]"),
        Command(*this, "DROP SEGMENT",     true,  2,  1,       &CP::com_drop_s,       "Drop a segment of a given partition by its name.", "DROP SEGMENT [str:partname] [str:segname]"),
        Command(*this, "INSERT TUPLE",     true,  2,  INVALID, &CP::com_insert_tuple, "", ""),
        Command(*this, "SHOW PARTITION",   true,  2,  1,       &CP::com_show_part,    "Show detailed information for a partition.", "SHOW PARTITION [str:partname]"),
        Command(*this, "SHOW PARTITIONS",  false, 2,  0,       &CP::com_show_parts,   "Show all partitions.", "SHOW PARTITIONS"),
        Command(*this, "SHOW SEGMENT",     true,  2,  2,       &CP::com_show_seg,     "Show detailed information for a segment.", "SHOW SEGMENT [str:partname] [str:segname]"),
        Command(*this, "SHOW SEGMENTS",    false, 2,  1,       &CP::com_show_segs,    "Show all segments.", "SHOW SEGMENTS"),
        Command(*this, "EXIT",             false, 1,  0,       &CP::com_exit,         "Shut down masterDB and exit.", "EXIT")
    },
    _maxCommandLength(0),
    _reader(),
    _init(false),
    _cb(nullptr)
{}

void CommandParser::init(const CB& aControlBlock, const char* aPrompt, const char aCommentChar) {
    if (!_init) {
        _reader.set_prompt(aPrompt);
        _reader.set_commentchar(aCommentChar);
        for (size_t i = 0; i < _commands.size(); ++i) {
            _maxCommandLength = strlen(_commands[i]._name) > _maxCommandLength ? strlen(_commands[i]._name) : _maxCommandLength;
        }
        _cb = &aControlBlock;
        //start thread
        runcli();
    }
}

void CommandParser::multiexec(const string_vt& commands) {
    for (const auto& line : commands) {
        const char_vpt& splits = _reader.split_line(' ', true, line);
        const Command* com = findCommand(&splits);
        if (com != NULL) {
            if ((splits.size() - com->_comLength) != com->_numParams && !(com->_numParams == INVALID)) {
                std::cout << "Wrong number of args.\n"
                          << "Usage:\n  " << com->_usageInfo << "\n"
                          << std::endl;
            } else {
                int rec;
                if (com->_hasParams) {
                    const char_vpt args(&splits[com->_comLength], &splits[splits.size()]);
                    rec = (this->*com->_func)(&args);
                } else
                    rec = (this->*com->_func)(nullptr);
                if (rec == CP::CommandStatus::EXIT || rec == CP::CommandStatus::ERROR) {
                    break;
                }
            }
        } else std::cout << "Invalid command" << std::endl;
    }
    DatabaseInstanceManager::getInstance().shutdown();
}

void CommandParser::runcli() {
    printw();
    for (_reader.open(); _reader.ok(); _reader.next()) {
        _reader.split_line(' ', true);
        const char_vpt& splits = _reader.splits();
        const Command* com = findCommand(&splits);
        if (com != NULL) {
            if (splits.size() == com->_comLength + 1 && *splits[splits.size() - 1] == *CP::_HELP_FLAG) {
                std::cout << "Help information for command: " << com->_name << "\n"
                          << "Purpose:\n  " << com->_helpMsg << "\n"
                          << "Usage:\n  " << com->_usageInfo << "\n"
                          << std::endl;
            } else if ((splits.size() - com->_comLength) != com->_numParams && !(com->_numParams == INVALID)) {
                std::cout << "Wrong number of args.\n"
                          << "Usage:\n  " << com->_usageInfo << "\n"
                          << std::endl;
            } else {
                int rec;
                if (com->_hasParams) {
                    const char_vpt args(&splits[com->_comLength], &splits[splits.size()]);
                    rec = (this->*com->_func)(&args);
                } else
                    rec = (this->*com->_func)(nullptr);
                if (rec == CP::CommandStatus::EXIT || rec == CP::CommandStatus::ERROR) {
                    break;
                } else if (rec == CP::CommandStatus::WRONGTYPE) {
                    std::cout << "Wrong type of some argument.\n"
                              << "Usage - " << com->_usageInfo << "\n"
                              << std::endl;
                } else if (rec == CP::CommandStatus::OK) {
                    continue;
                }
            }
        } else {
            std::cout << "Invalid command, type:\n"
                      << "  'HELP'       for a list of commands or\n"
                      << "  [COMMAND] " << _HELP_FLAG << " for information on a single command.\n"
                      << std::endl;
        }
    }
    DatabaseInstanceManager::getInstance().shutdown();
}

const CP::Command* CommandParser::findCommand(const char_vpt* splits) {
    std::string com(splits->at(0));
    std::string com_warg = "";
    if (splits->size() > 1) { com_warg = std::string(splits->at(0)) + " " + std::string(splits->at(1)); }
    std::transform(com.begin(), com.end(), com.begin(), ::toupper);
    std::transform(com_warg.begin(), com_warg.end(), com_warg.begin(), ::toupper);
    for (size_t i = 0; i < _commands.size() ; ++i) {
        std::string name(_commands[i]._name);
        if (name == com || name == com_warg) {
            return &_commands[i];
        }
    }
    return nullptr;
}

std::string CommandParser::findCommand(const std::string& arg) const {
    for (size_t i = 0; i < _commands.size(); ++i) {
        std::string name(_commands[i]._name);
        if (name == arg)
            return name;
    }
    return "";
}

int CP::com_help(const char_vpt* args) const {
    printh();
    return CP::CommandStatus::OK;
}

int CP::com_exit(const char_vpt* args) const {
    printe();
    return CP::CommandStatus::EXIT;
}

int CP::com_create_p(const char_vpt* args) const {
    std::string path(args->at(0));
    std::string partName(args->at(1));
    uint growthInd;
    if (!_reader.isnumber(args->at(2)))
        return CP::CommandStatus::WRONGTYPE;
    else growthInd = atoi(args->at(2));
    try {
        PartitionManager::getInstance().createPartitionFileInstance(path, partName, growthInd);
    } catch(const PartitionFullException& ex) {
        std::cout << "Partition Full." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(const fs::filesystem_error& fse) {
        std::cout << "FS Exception." << std::endl;
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_drop_p(const char_vpt* args) const {
    std::string partName(args->at(0));    
    try {
        PartitionManager::getInstance().deletePartition(partName);
    } catch(const std::out_of_range& oore) {
        std::cout << "Partition " << partName << " does not exist." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_create_s(const char_vpt* args) const {
    const std::string partName(args->at(0));
    const std::string segName(args->at(1));
    try {
        SegmentManager::getInstance().createNewSegmentFSM_SP(*PartitionManager::getInstance().getPartition(partName), segName);
    } catch(const std::out_of_range& oore) {
        std::cout << "Partition " << partName << " does not exist." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_drop_s(const char_vpt* args) const {
    std::string segName(args->at(0));
    try {
        SegmentManager::getInstance().deleteSegment(segName);
    } catch(const std::out_of_range& oore) {
        std::cout << "Segment " << segName << " does not exist." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_insert_tuple(const char_vpt* args) const {
    /* INSERT INTO Seg_Emp Employee Mueller 30 8000 */
    std::string segName(args->at(2));
    std::string type(args->at(3));
    // TODO rewrite this with templates
    if (type == "Employee") {
        if (args->size() != (4+3)) { /*handle*/ } // change to check for num args of Employee_T and num args of command INSERT INTO
        else {
            std::string emp_name(args->at(4));
            int emp_age = atoi(args->at(5));
            double emp_sal = atof(args->at(6));
            //Employee_T e(emp_name, emp_age, emp_sal);
            //SegmentManager::getInstance().getSegment(segName).insertTuple(e);
            return CP::CommandStatus::OK;
        }
    }
    return CP::CommandStatus::ERROR;
}

int CP::com_show_part(const char_vpt* args) const {
    std::string partName(args->at(0));
    try {
        const Partition_T& part = PartitionManager::getInstance().getPartitionT(partName);
        size_t partID = part.ID();
        const string_vt& segNames = SegmentManager::getInstance().getSegmentNamesForPartition(partID);
        std::cout << "PartitionID:    " << partID << std::endl;
        std::cout << "Partition:      " << part.name() << std::endl;
        std::cout << "Partition Type: " << part.type() << std::endl;
        if (segNames.size() == 0) {
            std::cout << "No Segments exist for Partition " << partName << "." << std::endl;
            return CP::CommandStatus::OK;
        } else {
            pprints(partName, segNames);
        }
    } catch(const std::out_of_range& oore) {
        std::cout << "Partition " << partName << " does not exist." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(...) {
        return CP::CommandStatus::ERROR;   
    }
    return CP::CommandStatus::OK;
}

int CP::com_show_parts(const char_vpt* args) const {
    try {
        const string_vt& names = PartitionManager::getInstance().getPartitionNames();
        const std::string& cap = "Partitions";
        pprints(cap, names);
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_show_seg(const char_vpt* args) const {
    std::string segName(args->at(0));
    try {
        const Segment_T& seg = SegmentManager::getInstance().getSegmentByName(segName);
        std::cout << "SegmentID:    " << seg.ID() << std::endl;
        std::cout << "Segment:      " << seg.name() << std::endl;
        std::cout << "Partition:    " << PartitionManager::getInstance().getPartitionName(seg.partID()) << std::endl;
        std::cout << "PartitionID:  " << seg.partID() << std::endl;
        std::cout << "Segment Type: " << seg.type() << std::endl;
    } catch(const std::out_of_range& oore) {
        std::cout << "Segment " << segName << " does not exist." << std::endl;
        return CP::CommandStatus::ERROR;
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

int CP::com_show_segs(const char_vpt* args) const {
    try {
        const string_vt& names = SegmentManager::getInstance().getSegmentNames();
        const std::string& cap = "Segments";
        pprints(cap, names);
    } catch(...) {
        return CP::CommandStatus::ERROR;
    }
    return CP::CommandStatus::OK;
}

void CommandParser::printw() const {
    std::cout << "Welcome to the command line interface of masterDB.\n\n"
              << "Type 'HELP' for help and usage information.\n"
              << "Type 'EXIT' for shutting down the database.\n"
              << "Type [COMMAND] " << _HELP_FLAG << " for further information on a single command.\n" << std::endl;
}

void CommandParser::printh() const {
    std::cout << "List of all masterDB commands:" << std::endl;
    for (size_t i = 0; i < _commands.size() ; ++i) {
        std::cout << "  " << std::setw(_maxCommandLength + 1) << std::left << _commands[i]._name << " " << _commands[i]._helpMsg << std::endl;
    }
    std::cout << "\n" << "For further information on a single command, type:\n"
                      << "  [COMMAND] " << _HELP_FLAG << "\n" << std::endl;
}

void CommandParser::printe() const {
    std::cout << "Good Bye.\n" << std::endl;
}

void CommandParser::pprints(const std::string& caption, const string_vt& list) const {
    uint8_t longestStr = caption.size();
    for (const std::string& line : list) {
        if (line.size() > longestStr)
            longestStr = line.size();
    }
    printp(longestStr);
    std::cout << "+ " << caption << " +" << std::endl;
    printp(longestStr);
    for (const std::string& line : list) {
        std::cout << "+ " << line;
        for (size_t i = 0; i < longestStr - line.size(); ++i)
            std::cout << " ";
        std::cout << " +" << std::endl;
    }
    printp(longestStr);
    std::cout << std::endl;
}

void CommandParser::printp(uint8_t length) const {
    for (uint8_t i = 0; i < length + 2; ++i)
        std::cout << "+";
    std::cout << std::endl;
}