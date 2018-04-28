#include "parser.hh"

const static Command commands[] = {
      // name, hasParams, comParams, numParams, func, helpMsg
      { "HELP", false, 1, 0, com_help, "This command displays usage information." },
      { "CREATE PARTITION", true, 2, 3, com_create_p, "" },
      { "DROP PARTITION", true, 2, 1, com_drop_p, "" },
      { "CREATE SEGMENT", true, 2, 2, com_create_s, "" },
      { "DROP SEGMENT", true, 2, 1, com_drop_s, "" },
      { "SHOW PARTITION", true, 2, 1, com_show_p, "" },
      { "SHOW SEGMENT", true, 2, 1, com_show_s, "" },
      { "EXIT", false, 1, 0, com_exit, "Shutting down masterDB." },
      // end sentinel
      { (char*)NULL, 0, 0, 0, ""} };

CommandParser::CommandParser() :
    _reader(),
    _init(false),
    _cb(nullptr)
{}

CommandParser::~CommandParser() {}

void CommandParser::init(const CB& aControlBlock, const char* aPrompt, const char aCommentChar) {
    if (!_init) {
        LineReaderEdit _reader(aPrompt, aCommentChar);
        _cb = &aControlBlock;
        _init = true;
    }
}

void CommandParser::runcli() {
    printw();

    for (_reader.open(); _reader.ok(); _reader.next()) {
        _reader.split_line(' ', true);
        const char_vpt splits = _reader.splits();
        Command* com = findCommand(splits);

        if (com != NULL) {
            if ((splits.size() - com->_comParams) != com->_numParams) {
                // wrong number of args, display help for this command
            } else {

                // if has no args, pass null
                com->_func(splits);
            }
        } else { // invalid command

        }
    }
    _reader.close();
}

Command* CommandParser::findCommand(const std::vector<char*>& splits) {
    const char* com = splits.at(0);
    std::string com_warg = std::string(splits.at(0)) + " " + std::string(splits.at(1));

    for (size_t i = 0; commands[i]._name != NULL; ++i) {
        if (commands[i]._name == com || commands[i]._name == com_warg) return &commands[i];
    }
    return (Command*) 0;
}

void CommandParser::printw() {
    std::cout << "Welcome to the command line interface of masterDB. "
        << "Commands end with ;.\n\n"
        << "Type 'help;' for help and usage information.\n"
        << "Type 'exit;' for shutting down the database.\n\n"
        << _reader.prompt();
}

void CommandParser::printh() {
    for (size_t i = 0; commands[i]._name != NULL; ++i) {
        std::cout << commands[i]._name << ": " << commands[i]._helpMsg << std::endl;
    }
}

void CommandParser::printe() {
    std::cout << "Good Bye." << std::endl;
}

static int com_help(const char_vpt& splits) {
  printh();
  return 1;
}

static int com_exit(const char_vpt& splits) {
  DatabaseInstanceManager::getInstance().shutdown();
  printe();
  _reader.close();
  return 1;
}

static int com_create_p(const char_vpt& splits) {}
static int com_drop_p(const char_vpt& splits) {}
static int com_create_s(const char_vpt& splits) {}
static int com_drop_s(const char_vpt& splits) {}
static int com_show_p(const char_vpt& splits) {}
static int com_show_s(const char_vpt& splits) {}