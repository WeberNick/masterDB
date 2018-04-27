/**
 *  @author Nick Weber
 *  @brief  Implements tracing functionality
 *  @todo   If multi threaded access causes problems, add mutex
 *  @bugs   -
 *
 *  @section DESCRIPTION
 *  todo 
 *  @section USE
 *  todo
 */
#pragma once

#include "types.hh"
#include "exception.hh"
#include <string>
#include <fstream>
#include <ctime>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define TRACE(msg) Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg)

class Trace
{
    private:
        explicit Trace();
        explicit Trace(const Trace&) = delete;
        explicit Trace(Trace&&) = delete;
        Trace& operator=(const Trace&) = delete;
        Trace& operator=(Trace&&) = delete;
        ~Trace();

    public:
        inline static Trace& getInstance()
        {
            static Trace lInstance;
            return lInstance;
        }

        void init(const CB& aControlBlock);

    public:
        void log(const char* aFileName, const uint aLineNumber, const char* aFunctionName, const std::string& aMessage);

    public:
        inline const std::string&   getLogPath(){ return _logPath; }
        inline const std::ofstream& getLogStream(){ return _logStream; }

    private:
        std::string     _logPath;
        std::ofstream   _logStream;
        const CB*       _cb;
        bool            _init;
};

