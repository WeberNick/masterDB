/**
 * @file   parser.hh
 * @author Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief  Class implementing the parser for the command line interface
 * @bugs   TBD
 * @todos  TBD
 */
#pragma once

#include "../infra/linereaderedit.hh"
#include "../infra/types.hh"

class CommandParser {
  
  struct Command {
    const char* _name;
    // fct pointer
    bool _hasParams;
    size_t _maxParams;
    std::string _printMsg;
  }

  protected:
    explicit CommandParser();
    explicit CommandParser(const CommandParser&) = delete;
    explicit CommandParser(CommandParser&&) = delete;
    CommandParser& operator=(const CommandParser&) = delete;
    CommandParser& operator=(CommandParser&&) = delete;
    ~CommandParser();

  private:
    void printWelcome();
    void printHelp();    
    void printExit();

  public:
    static CommandParser& getInstance() {
        static CommandParser lComPars;
        return lComPars;
    }

    void init(const CB& aControlBlock, const char* aPrompt = "> ", const char aCommentChar = '#');

  public:
    void runcli();

  private:
    static Command _com_help;
    static const char* _com_exit;
    static const char* _com_createpart;
    static const char* _com_createsegm;

    LineReaderEdit _reader;

    bool _init;
    const CB* _cb;
};