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

#include <array>

class CommandParser;
using CP = CommandParser;

class CommandParser {
  private:
    class Command {
      public:
        explicit Command(const CP& aCP,
                         const char* aName,
                         const bool aHasParams,
                         const size_t aCommandLength,
                         const size_t aNumParams,
                         int (CP::*aFunc)(const char_vpt*) const,
                         const char* aMsg,
                         const char* aUsageInfo);
        Command& operator=(const Command& aCommand);

      public:                                    //           Example
        const CP& _cp;                           //                            - Reference to the Command Parser instance
        const char* _name;                       // "CREATE SEGMENT"           - The name of the command
        bool _hasParams;                         // true                       - Whether the command has parameters
        size_t _comLength;                       // 2                          - The number of words the command consists of (CREATE and SEGMENT)
        size_t _numParams;                       // 2                          - The number of parameters
        int (CP::*_func)(const char_vpt*) const; // com_create_s               - A function pointer implementing the logic of this command
        const char* _helpMsg;                    // "Create .."                - A help message
        const char* _usageInfo;                  // "Usage - str: partname .." - A detailed message on how to use the command
    };

    enum CommandStatus {
      ERROR = -1,
      OK = 0,
      EXIT = 1,
      WRONGTYPE = 2
    };

  public:
    explicit CommandParser();
    explicit CommandParser(const CommandParser&) = delete;
    explicit CommandParser(CommandParser&&) = delete;
    CommandParser& operator=(const CommandParser&) = delete;
    CommandParser& operator=(CommandParser&&) = delete;
    ~CommandParser() = default;

  private:
    void runcli();
    const Command* findCommand(const std::vector<char*>& splits);
    std::string findCommand(std::string& arg) const;

    void printw() const;
    void printh() const;
    void printe() const;

  private:
    int com_help(const char_vpt* args) const;
    int com_exit(const char_vpt* args) const;
    int com_create_p(const char_vpt* args) const;
    int com_drop_p(const char_vpt* args) const;
    int com_create_s(const char_vpt* args) const;
    int com_drop_s(const char_vpt* args) const;
    int com_show_p(const char_vpt* args) const;
    int com_show_s(const char_vpt* args) const;

  public:
    static CommandParser& getInstance() {
        static CommandParser lComPars;
        return lComPars;
    }

    void init(const CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#');

  private:
    static const char* _HELP_FLAG;
    const std::array<const Command, 8> _commands;
    size_t _maxCommandLength;

    LineReaderEdit _reader;
    bool _init;
    const CB* _cb;
};
