#include "exception.hh"

BaseException::BaseException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const char*         aErrorMessage) :
    std::runtime_error(aErrorMessage),
	_file(aFileName),
	_line(aLineNumber),
	_func(aFunctionName)
	//_errMsg(aErrorMessage)
{}

BaseException::BaseException(
        const std::string&  aFileName,
        const unsigned int  aLineNumber,
        const std::string&  aFunctionName,
        const std::string&  aErrorMessage) :
    std::runtime_error(aErrorMessage),
	_file(aFileName),
	_line(aLineNumber),
	_func(aFunctionName)
{}

BaseException::BaseException(const BaseException& aException) :
    std::runtime_error(aException.what()),
	_file(aException._file),
	_line(aException._line),
	_func(aException._func)
{}

void BaseException::print() const
{
    std::cerr << "An error occured in " << _file
        << ", line " << _line
        << ", '" << _func
        << "':\n\t\"" << what() << "\""
        << std::endl;
}

ReturnException::ReturnException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            "Can not recover from exception. Programm will terminate.")
{}


SingletonException::SingletonException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            "Cannot return reference to uninstantiated singleton.")
{}

OutOfMemoryException::OutOfMemoryException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            "There was insufficient memory to fulfill the allocation request.")
{}

PartitionException::PartitionException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            aErrorMessage)
{}

PartitionExistsException::PartitionExistsException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            std::string("Cannot create existing partition or remove non existing partition"))
{}

PartitionFullException::PartitionFullException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        byte*               aBufferPointer,
        const uint          aIndexOfFSIP) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            "The partition is full. Can not allocate any new pages."),
    _bufPtr(aBufferPointer), 
    _index(aIndexOfFSIP)
{}

PartitionFullException::PartitionFullException(const PartitionFullException& aOther) : 
    BaseException(aOther),
    _bufPtr(aOther.getBufferPtr()),
    _index(aOther.getIndexOfFSIP())
{}


FSIPException::FSIPException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            aErrorMessage)
{}

NSMException::NSMException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            aErrorMessage)
{}

InvalidArgumentException::InvalidArgumentException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            aErrorMessage)
{}

SwitchException::SwitchException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            std::string("Not allowed access to default case of switch-statement: ") + aErrorMessage)
{}

FileException::FileException(
        const char*         aFileName,
        const unsigned int  aLineNumber,
        const char*         aFunctionName,
        const char*         aErrorFileName,
        const std::string&  aErrorMessage) :
	BaseException(
            aFileName,
            aLineNumber,
            aFunctionName,
            std::string("Problem with file ") + std::string(aErrorFileName) + std::string(" : ") + aErrorMessage)
{}
