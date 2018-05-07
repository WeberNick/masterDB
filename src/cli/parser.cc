#include "parser.hh"

CP::CMD::Command(
                const CP& aCP, 
                const char* aName, 
                const bool aHasParams, 
                const size_t aCommandLength, 
                const size_t aNumParams, 
                int (CP::*aFunc)(const char_vpt&) const,
                const char* aMsg) :
    _cp(aCP), _name(aName), _hasParams(aHasParams), _comLength(aCommandLength), _numParams(aNumParams), _func(aFunc), _helpMsg(aMsg)
{}

CP::CMD& CP::CMD::operator=(const Command& aCMD){}

CommandParser::CommandParser() :
    _commands(),
    _reader(),
    _maxCommandLength(0),
    _exit(false),
    _init(false),
    _cb(nullptr)
{
    //_commands = ;

}

CommandParser::~CommandParser() {}

void CommandParser::init(const CB& aControlBlock, const char* aPrompt, const char aCommentChar) {
    if (!_init) {
        _commands = {
            Command(*this, "HELP", false, 1, 0, &CP::com_help, "This command displays usage information."),
            Command(*this, "CREATE PARTITION", true, 2, 3, &CP::com_create_p, ""),
            Command( *this, "DROP PARTITION", true, 2, 1, &CP::com_drop_p, ""),
            Command( *this, "CREATE SEGMENT", true, 2, 2, &CP::com_create_s, ""),
            Command( *this, "DROP SEGMENT", true, 2, 1, &CP::com_drop_s, "" ),
            Command( *this, "SHOW PARTITION", true, 2, 1, &CP::com_show_p, "" ),
            Command( *this, "SHOW SEGMENT", true, 2, 1, &CP::com_show_s, "" ),
            Command( *this, "EXIT", false, 1, 0, &CP::com_exit, "Shutting down masterDB." )
       };
        LineReaderEdit _reader(aPrompt, aCommentChar);
        for (size_t i = 0; _commands[i]._name != NULL; ++i) {
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
    for (_reader.open(); _reader.ok() && !(_exit); _reader.next()) {
        _reader.split_line(' ', true);
        const char_vpt splits = _reader.splits();
        Command* com = findCommand(splits);

        if (com != NULL) {
            if ((splits.size() - com->_comLength) != com->_numParams) {
                // wrong number of args, display help for this command
            } else {

                // if has no args, pass null
                //( obj.*f ) ( 123 );
                //(this->*com->_func)(splits);

                if(!(((com->_cp).*com->_func)(splits)))
                {
                    _exit = true;
                }
            }
        } else { // invalid command
        }
    }
    close();
}

void CommandParser::close() {
    //DatabaseInstanceManager::getInstance().shutdown();
    _reader.close();
    printe();
}


CP::CMD* CommandParser::findCommand(const std::vector<char*>& splits) {
    const char* com = splits.at(0);
    std::string com_warg;
    if (splits.size() > 1) { com_warg = std::string(splits.at(0)) + " " + std::string(splits.at(1)); }
    for (size_t i = 0; _commands[i]._name != NULL; ++i) {
        if (_commands[i]._name == com || _commands[i]._name == com_warg) return &_commands[i];
    }
    return nullptr;
}

void CommandParser::printw() const {
    std::cout << "Welcome to the command line interface of masterDB. "
              << "Commands end with ;.\n\n"
              << "Type 'help;' for help and usage information.\n"
              << "Type 'exit;' for shutting down the database.\n\n"
              << _reader.prompt();
}

void CommandParser::printh() const {
    for (size_t i = 0; _commands[i]._name != NULL; ++i) {
        std::cout << _commands[i]._name << ": " << _commands[i]._helpMsg << std::endl;
    }
}

void CommandParser::printe() const { std::cout << "Good Bye." << std::endl; }

 int CP::com_help(const char_vpt& args) const {
    printh();
    return 1;
}

 int CP::com_exit(const char_vpt& args) const {
    printe();
    return 0;
}

 int CP::com_create_p(const char_vpt& args) const { return 0; }
 int CP::com_drop_p(const char_vpt& args) const { return 0; }
 int CP::com_create_s(const char_vpt& args) const { return 0; }
 int CP::com_drop_s(const char_vpt& args) const { return 0; }
 int CP::com_show_p(const char_vpt& args) const { return 0; }
 int CP::com_show_s(const char_vpt& args) const { return 0; }
