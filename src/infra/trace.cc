#include "trace.hh"

Trace::Trace() :
    _logPath(),
    _logStream(),
    _cb(nullptr),
    _init(false)
{}

Trace::~Trace()
{
    if(_cb->trace())
    {
        TRACE("Closing the log file...");
        _logStream.close();
        TRACE("Log file closed!");
    }
    TRACE("'Trace' destructed");
}

void Trace::init(const CB& aControlBlock) noexcept
{
    if(!_init)
    {
        _cb = &aControlBlock;
        _logPath = _cb->tracePath();
        _logPath.append("logs/");
        fs::create_directory(_logPath);
        std::time_t lCurrTime = std::time(nullptr);
        std::string lTime = std::string(std::ctime(&lCurrTime));
        _logPath.append(lTime.substr(0, lTime.size() - 1).append(".log"));
        if(_cb->trace())
        {
            _logStream.open(_logPath.c_str(), std::ofstream::out | std::ofstream::app);
            TRACE("'Trace' constructed"); //just for consistency with the other singletons
            TRACE("Log file created and opened");
        }
        _init = true;
        TRACE("'Trace' initialized");
    }
}

void Trace::log(const char* aFileName, const uint aLineNumber, const char* aFunctionName, const std::string& aMessage) noexcept
{
    if(_cb->trace())
    {
        std::time_t lCurrTime = std::time(nullptr);
        std::string lTime = std::ctime(&lCurrTime);
        lTime = lTime.substr(0, lTime.size() - 1);
        _logStream << lTime 
            << ": " << aFileName 
            << ", line " << aLineNumber
            << ", " << aFunctionName
            << ": '" << aMessage << "'"
            << std::endl;
    }
}

