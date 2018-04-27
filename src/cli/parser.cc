#include "parser.hh"



CommandParser::CommandParser() :
    _reader(),
    _cb(nullptr),
    _init(false)
{}

CommandParser::~CommandParser() {}

void CommandParser::init(const CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#') {
    if (!_init) {
        _reader = LineReaderEdit(aPrompt, aCommentChar);
        _cb = &aControlBlock;
        _init = true;
    }
}

void CommandParser::runcli() {
    // start thread for cli
    printWelcome();
    const char* line;
    const char* beg;
    const char* end;

    for (_reader.open(); _reader.ok(); _reader.next()) {
        line = _reader.line();
        _reader.skipws(line);
        _reader.read_string_no_delim(line, ' ', beg, end);
        std::cout << "the command is: " << std::string(beg, end) << std::endl;
    }
    // _reader.close();
}

void printWelcome() {
    std::cout << "Welcome to the command line interface of masterDB. "
        << "Commands end with ;.\n\n"
        << "Type 'help;' for help and usage information.\n"
        << "Type 'exit;' for shutting down the database.\n\n"
        << _reader.prompt();
}

void printHelp() {
    // TODO
    std::cout << "halp" << std::endl;
}

void printExit() {
    std::cout << "Good Bye." << std::endl;
}