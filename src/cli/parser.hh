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
#include "../main/db_instance_manager.hh"
#include "../partition/partition_manager.hh"
#include "../segment/segment_manager.hh"

class CommandParser {
  private:
    struct Command {
        const char* _name;
        bool _hasParams;
        size_t _comLength;
        size_t _numParams;
        int (*_func)(const char_vpt&);
        const char* _helpMsg;
    };

  public:
    explicit CommandParser();
    explicit CommandParser(const CommandParser&) = delete;
    explicit CommandParser(CommandParser&&) = delete;
    CommandParser& operator=(const CommandParser&) = delete;
    CommandParser& operator=(CommandParser&&) = delete;
    ~CommandParser();

  private:
    void runcli();
    Command* findCommand(const std::vector<char*>& splits);
    void printw();
    void printh();
    void printe();
    void close();

  private:
    static int com_help(const char_vpt& args);
    static int com_exit(const char_vpt& args);
    static int com_create_p(const char_vpt& args);
    static int com_drop_p(const char_vpt& args);
    static int com_create_s(const char_vpt& args);
    static int com_drop_s(const char_vpt& args);
    static int com_show_p(const char_vpt& args);
    static int com_show_s(const char_vpt& args);

  public:
    static CommandParser& getInstance() {
        static CommandParser lComPars;
        return lComPars;
    }

    void init(const CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#');

  private:
    static std::vector<Command> _commands;
    LineReaderEdit _reader;

    size_t _maxCommandLength;
    bool _exit;
    bool _init;
    const CB* _cb;
};