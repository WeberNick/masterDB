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
#include "../partition/partition_manager.hh"
#include "../segment/segment_manager.hh"

struct Command {
    const char* _name;
    bool _hasParams;
    size_t _comLength;
    size_t _numParams;
    int (*_func)(char_vpt&);
    const char* _helpMsg;
};

class CommandParser {
  protected:
    explicit CommandParser();
    explicit CommandParser(const CommandParser&) = delete;
    explicit CommandParser(CommandParser&&) = delete;
    CommandParser& operator=(const CommandParser&) = delete;
    CommandParser& operator=(CommandParser&&) = delete;
    ~CommandParser();

  private:
    Command* findCommand(const std::vector<char*>& splits);
    void printw();
    void printh();
    void printe();
  
  private:
    static int com_help(const char_vpt& splits);
    static int com_exit(const char_vpt& splits);
    static int com_create_p(const char_vpt& splits);
    static int com_drop_p(const char_vpt& splits);
    static int com_create_s(const char_vpt& splits);
    static int com_drop_s(const char_vpt& splits);
    static int com_show_p(const char_vpt& splits);
    static int com_show_s(const char_vpt& splits);

  public:
    static CommandParser& getInstance() {
        static CommandParser lComPars;
        return lComPars;
    }

    void init(const CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#');
    void runcli();

  private:
    LineReaderEdit _reader;

    bool _init;
    const CB* _cb;
};