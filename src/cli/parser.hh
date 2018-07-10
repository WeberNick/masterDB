/**
 * @file    parser.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief   Class implementing the parser for the command line interface
 * @bugs    Currently no bugs known
 * @todos   -
 * @section TODO
 */

#pragma once

#include "../infra/employee_t.hh"
#include "../infra/exception.hh"
#include "../infra/partition_t.hh"
#include "../infra/segment_t.hh"
#include "../infra/types.hh"
#include "../main/db_instance_manager.hh"
#include "../partition/partition_manager.hh"
#include "../segment/segment_manager.hh"
#include "../threading/pool.hh"
#include "linereaderedit.hh"

#include <algorithm>
#include <array>
#include <string>
#include <functional>

class CommandParser;
using CP = CommandParser;

class CommandParser
{
    private:
        class Command
        {
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

            public:                                      // Examples:
                const CP& _cp;                           //                            - Reference to the Command Parser instance
                const char* _name;                       // "CREATE SEGMENT"           - The name of the command
                bool _hasParams;                         // true                       - Whether the command has parameters
                size_t _comLength;                       // 2                          - The number of words the command consists of (CREATE and SEGMENT)
                size_t _numParams;                       // 2                          - The number of parameters
                int (CP::*_func)(const char_vpt*) const; // com_create_s               - A function pointer implementing the logic of this command
                const char* _helpMsg;                    // "Create .."                - A help message
                const char* _usageInfo;                  // "Usage - str: partname .." - A detailed message on how to use the command
    };

        enum CommandStatus
        {
            EXIT  = -1, // regular exit
            OK    = 0,  // ok, continue with next command
            WRONG = 1,  // wrong type of an argument
            ERROR = 2   // error (not fatal), recover and continue
        };

    public:
        explicit CommandParser();
        explicit CommandParser(const CommandParser&) = delete;
        explicit CommandParser(CommandParser&&) = delete;
        CommandParser& operator=(const CommandParser&) = delete;
        CommandParser& operator=(CommandParser&&) = delete;
        ~CommandParser() = default;

    private:
        /**
         * @brief run the command line interface
         * 
         */
        void runcli();
        /**
         * @brief find a command for
         * 
         * @param splits 
         * @return const Command* 
         */
        const Command* findCommand(const char_vpt* splits);
        /** TODO
         * @brief 
         * 
         * @param arg 
         * @return std::string 
         */
        std::string findCommand(const std::string& arg) const;
    
        /** TODO
         * @brief 
         * 
         */
        void printw() const;
        /** TODO
         * @brief 
         * 
         */
        void printh() const;
        /** TODO
         * @brief 
         * 
         */
        void printe() const;
        
        template <typename T>
        void pprintelems(const std::vector<T>& tuples) const;
        void pprintelem(const string_vt& values, const std::vector<uint8_t>& spaces) const;
        void printptable(const std::vector<uint8_t>& spaces) const;

    private:
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_help(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_create_p(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_drop_p(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_create_s(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_drop_s(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_insert_tuple(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_show_part(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_show_parts(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_show_seg(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_show_segs(const char_vpt* args) const;
        /** TODO
         * @brief 
         * 
         * @param args 
         * @return int 
         */
        int com_exit(const char_vpt* args) const;

    public:
        /**
         * @brief execute multiple CLI commands in an automated fashion to test multithreaded execution
         * 
         * @param commands the command list
         */
        void multiexec(const string_vt& commands);

    public:
        /**
         * @brief Get the CommandParser instance object
         * 
         * @return CommandParser& the CommandParser instance
         */
        static CommandParser& getInstance()
        {
            static CommandParser lComPars;
            return lComPars;
        }
        /**
         * @brief Initialize the control block
         * 
         * @param aControlBlock the control block
         * @param aPrompt the prompt in the command line interface, defaults to " > "
         * @param aCommentChar the comment char in the command line interface, defaults to " # "
         */
        void init(const CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#');

    private:
        static const char*                  _HELP_FLAG;
        const std::array<const Command, 11> _commands;
        size_t                              _maxCommandLength;
    
        LineReaderEdit _reader;
        const CB*      _cb;
};
