/**
 * @file    parser.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief   Class implementing the parser for the command line interface
 * @bugs    Currently no bugs known
 * @todos   -
 * @section DESCRIPTION
 *  This class implements a command parser which serves as an interface for the user to control the system
 *  and execute different tasks. Among the offered functionalities are: creation of partition/segment, insertion of 
 *  tuples, displaying detailed information about different entities, and shutting down the database into a
 *  persistent state.
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
        enum class CommandStatus: int8_t
        {
            EXIT          = -1,  // regular exit
            CONTINUE      =  0,  // ok, continue with next command
            WRONG         =  1,  // wrong type of an argument
            UNKNOWN_ERROR =  2   // unknown error, irregular exit
        };

        class Command
        {
            public:
                Command(const CP& aCP,
                    const char* aName,
                    const bool aHasParams,
                    const size_t aCommandLength,
                    const size_t aNumParams,
                    CP::CommandStatus (CP::*aFunc)(const char_vpt*) const,
                    const char* aMsg,
                    const char* aUsageInfo);
                Command& operator=(const Command& aCommand);

            public:                                                // Examples:
                const CP& _cp;                                     //                            - Reference to the Command Parser instance
                const char* _name;                                 // "CREATE SEGMENT"           - The name of the command
                bool _hasParams;                                   // true                       - Whether the command has parameters
                size_t _comLength;                                 // 2                          - The number of words the command consists of (CREATE and SEGMENT)
                size_t _numParams;                                 // 2                          - The number of parameters
                CP::CommandStatus (CP::*_func)(const char_vpt*) const; // com_create_s               - A function pointer implementing the logic of this command
                const char* _helpMsg;                              // "Create .."                - A help message
                const char* _usageInfo;                            // "Usage - str: partname .." - A detailed message on how to use the command
        };

    public:
        CommandParser();
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
         * @brief detect a command from the given line which was typed in by the user
         * 
         * @param splits an array of char* containing the split up user command
         * @return const Command* the corresponding (valid) command
         */
        const Command* findCommand(const char_vpt* splits);
        /** 
         * @brief helper method to determine whether a certain command is supported
         *        and thus is contained in the commands array
         * 
         * @param arg the command to check
         * @return std::string the command string
         */
        std::string findCommand(const std::string& arg) const;
    
        /**
         * @brief print a welcome message
         * 
         */
        void printw() const;
        /** 
         * @brief print a help message
         * 
         */
        void printh() const;
        /**
         * @brief print an exit message
         * 
         */
        void printe() const;
        
        /**
         * @brief pretty print elements of parameter vector tuples
         * 
         * @tparam T the type of the elements to be printed
         * @param tuples the elements to print
         */
        template <typename T>
        void pprintelems(const std::vector<T>& tuples) const;
        /**
         * @brief pretty print an element, helper function for pprintelems
         * 
         * @param values the values of the element
         * @param spaces the positions of separation characters ("|")
         */
        void pprintelem(const string_vt& values, const std::vector<uint8_t>& spaces) const;
        /**
         * @brief pretty print a horizontal line
         * 
         * @param spaces the posistions of separation characters ("+")
         */
        void printptable(const std::vector<uint8_t>& spaces) const;

    private:
        /**
         * @brief implementing command functionality for installling the dbs
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_install(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for booting the dbs
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_boot(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for shutting down the dbs
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_shutdown(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for printing a detailed help information
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_help(const char_vpt* args) const;
        /** 
         * @brief implementing command functionality for creating a partition
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_create_p(const char_vpt* args) const;
        /** 
         * @brief implementing command functionality for deleting a partition
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_drop_p(const char_vpt* args) const;
        /** 
         * @brief implementing command functionality for creating a segment
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_create_s(const char_vpt* args) const;
        /** 
         * @brief implementing command functionality for deleting a segment
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_drop_s(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for inserting a tuple into a relation
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_insert_tuple(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for showing information for a specific partition
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_show_part(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for showing information for all partitions
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_show_parts(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for showing information for a specific segment
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_show_seg(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for showing information for all segments
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_show_segs(const char_vpt* args) const;
        /**
         * @brief implementing command functionality for exiting the system
         * 
         * @param args the argument vector consisting of the processed user input
         * @return CommandStatus the return code
         */
        CommandStatus com_exit(const char_vpt* args) const;

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
        void init(CB& aControlBlock, const char* aPrompt = "mdb > ", const char aCommentChar = '#');

    private:
        static const char*                  _HELP_FLAG;
        const std::array<const Command, 14> _commands;
        size_t                              _maxCommandLength;
    
        LineReaderEdit _reader;
        CB*            _cb;
};
