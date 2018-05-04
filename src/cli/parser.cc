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

CP::Command& CP::Command::operator=(const Command& aCommand) {
    return *this = &aCommand;
}

const char* CP::_HELP_FLAG = "-h";

CommandParser::CommandParser() : 
    _commands(),
    _maxCommandLength(0),
    _reader(), 
    _init(false),
    _cb(nullptr)
{}

CommandParser::~CommandParser() {}

void CommandParser::init(const CB& aControlBlock, const char* aPrompt, const char aCommentChar) {
    if (!_init) {
        _commands = { Command(*this, "HELP", false, 1, 0, &CP::com_help, "Displays usage information.", "HELP"),
                      Command(*this, "CREATE PARTITION", true, 2, 3, &CP::com_create_p, "Create a partition at a given destination path with a name and a growth indicator.", "CREATE PARTITION [str:path] [str:name] [int:growth_indicator]"),
                      Command(*this, "DROP PARTITION", true, 2, 1, &CP::com_drop_p, "Drop a partition by name.", "DROP PARTITION [str:name]"),
                      Command(*this, "CREATE SEGMENT", true, 2, 2, &CP::com_create_s, "Create a segment for a given partition with a name.", "CREATE SEGMENT [str:partname] [str:segname]"),
                      Command(*this, "DROP SEGMENT", true, 2, 1, &CP::com_drop_s, "Drop a segment of a given partition by its name.", "DROP SEGMENT [str:partname] [str:segname]"),
                      Command(*this, "SHOW PARTITION", true, 2, 1, &CP::com_show_p, "Show detailed information of a partition.", "SHOW PARTITION [str:partname]"),
                      Command(*this, "SHOW SEGMENT", true, 2, 2, &CP::com_show_s, "Show detailed information of a segment.", "SHOW SEGMENT [str:partname] [str:segname]"),
                      Command(*this, "EXIT", false, 1, 0, &CP::com_exit, "Shut down masterDB and exit.", "EXIT") };
        _reader.set_prompt(aPrompt);
        _reader.set_commentchar(aCommentChar);
        for (size_t i = 0; i < _commands.size(); ++i) {
            _maxCommandLength = strlen(_commands[i]._name) > _maxCommandLength ? strlen(_commands[i]._name) : _maxCommandLength;
        }
        _cb = &aControlBlock;
        _init = true;
    }
    // start thread
    runcli();
}

void CommandParser::runcli() {
    printw();
    for (_reader.open(); _reader.ok(); _reader.next()) {
        _reader.split_line(' ', true);
        const char_vpt splits = _reader.splits();
        Command* com = findCommand(splits);
        if (com != NULL) {
            FIX DROP SEGMENT -h (2,1)
            if ((splits.size() - com->_comLength) != com->_numParams) {
                if (splits.size() == com->_comLength + 1 && *splits[splits.size() - 1] == *CP::_HELP_FLAG) {
                    std::cout << "Help information for command: " << com->_name << std::endl;
                    std::cout << "Purpose:\n  " << com->_helpMsg << std::endl;
                } else {
                    std::cout << "Wrong number of args." << std::endl; 
                }
                std::cout << "Usage:\n  " << com->_usageInfo << "\n" << std::endl;
            } else {
                int rec;
                if (com->_hasParams) {
                    const char_vpt args(&splits[com->_comLength], &splits[splits.size()]);
                    rec = (this->*com->_func)(&args);
                }
                else rec = (this->*com->_func)(nullptr);
                if (rec == CP::CommandStatus::EXIT) {
                    break;
                } else if (rec == CP::CommandStatus::WRONGTYPE) {
                    std::cout << "Wrong type of some argument." << std::endl;
                    std::cout << "Usage - " << com->_usageInfo << std::endl;
                }
            }
        } else {
            if (splits.size() > 0) {
                std::string arg(splits.at(0));
                if (splits.size() > 1) arg += " " + std::string(splits.at(1));
                std::transform(arg.begin(), arg.end(), arg.begin(), ::toupper);
                std::string com = findCommand(arg);

                std::cout << "Invalid command, ";
                if (!com.empty()) {
                    std::cerr << "did you mean: \e[1m" << com << "\e[0m";
                } else {
                    std::cout << "type:\n"
                              << "  'HELP'       for a list of commands or\n"
                              << "  [COMMAND] " << _HELP_FLAG << " for information on a single command.";
                }
                std::cout << "\n" << std::endl;
            } else continue;
        }
    }
    // DatabaseInstanceManager::getInstance().shutdown();
}

CP::Command* CommandParser::findCommand(const std::vector<char*>& splits) {
    std::string com(splits.at(0));
    std::string com_warg = "";
    if (splits.size() > 1) { com_warg = std::string(splits.at(0)) + " " + std::string(splits.at(1)); }
    for (size_t i = 0; i < _commands.size() ; ++i) {
        std::string name(_commands[i]._name);
        if (name == com || name == com_warg) {
            return &_commands[i];
        }
    }
    return nullptr;
}

std::string CommandParser::findCommand(std::string& arg) const {
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
    //validate args
    //std::string path = args.at(0);
    //std::string name = args.at(1);
    //uint growthInd = args.at(2);
    //try{
    // PartitionManager::getInstance().createPartition(path, name, growthInd);
    //} catch (...) {
    //    return CP::CommandStatus::ERROR;
    //}
    return 0;
}

int CP::com_drop_p(const char_vpt* args) const {
    //validate args
    
    // PartitionManager::getInstance().
    return 0;
}

int CP::com_create_s(const char_vpt* args) const {
    //validate args
    
    return 0;
}

int CP::com_drop_s(const char_vpt* args) const {
    //validate args
    
    return 0;
}

int CP::com_show_p(const char_vpt* args) const {
    //validate args
    
    return 0;
}

int CP::com_show_s(const char_vpt* args) const {
    //validate args
    
    return 0;
}

void CommandParser::printw() const {
    std::cout << "Welcome to the command line interface of masterDB.\n\n"
              // << "Commands end with ;."
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

void CommandParser::printe() const { std::cout << "Good Bye.\n" << std::endl; }