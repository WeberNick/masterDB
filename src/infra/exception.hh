/**
 *	@file 	exception.hh
 *	@author	Nick Weber (nickwebe@pi3.informatik.uni-mannheim.de)
 *	@brief	Provides a central class for exception handling
 *	@bugs 	Currently no bugs known
 *	@todos  -	
 *
 *	@section DESCRIPTION
 *	TODO
 *  @section USE
 *	    throw BaseException(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Error Message");
 */
#pragma once

#include "types.hh"
#include <exception>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>

#define FLF __FILE__, __LINE__, __PRETTY_FUNCTION__

class BaseException : public std::runtime_error 
{
    public:
        explicit BaseException(
                const char*         aFileName, 
                const unsigned int  aLineNumber, 
                const char*         aFunctionName, 
                const char*         aErrorMessage); 
        explicit BaseException(
                const std::string&  aFileName, 
                const unsigned int  aLineNumber, 
                const std::string&  aFunctionName, 
                const std::string&  aErrorMessage); 
        explicit BaseException(const BaseException& aException);
        explicit BaseException(BaseException&&) = delete;
        BaseException& operator=(const BaseException&) = delete;
        BaseException& operator=(BaseException&&) = delete;
        virtual ~BaseException() = default;

    public:
//        virtual const char* what() const; // inherited from std::runtime_error
        void print() const;

    private:
        const std::string   _file;
        const unsigned int  _line;
        const std::string   _func;
};

class ReturnException : public BaseException 
{
    public:
        ReturnException(
                const char*         aFileName,
                const unsigned int  aLineNumber,
                const char*         aFunctionName);
};

class SingletonException : public BaseException
{
    public:
        SingletonException(
                const char*         aFileName,
                const unsigned int  aLineNumber,
                const char*         aFunctionName);
};

class OutOfMemoryException : public BaseException
{
	public:
		OutOfMemoryException(
                const char*         aFileName, 
                const unsigned int  aLineNumber, 
                const char*         aFunctionName);
};

class PartitionFullException : public BaseException
{
    public:
        PartitionFullException(
                const char*         aFileName,
                const unsigned int  aLineNumber,
                const char*         aFunctionName,
                byte*               aBufferPointer,
                const uint          aIndexOfFSIP);
        PartitionFullException(const PartitionFullException& aOther);
        PartitionFullException& operator=(const PartitionFullException&) = delete;

    public:
        inline byte* getBufferPtr() const { return _bufPtr; }
        inline uint  getIndexOfFSIP() const { return _index; }

    private:
        byte*   _bufPtr;
        uint    _index;
};

class FSIPException : public BaseException
{
    public:
        FSIPException(
                const char*         aFileName,
                const unsigned int  aLineNumber,
                const char*         aFunctionName,
                const std::string&  aErrorMessage);
};

class NSMException : public BaseException
{
    public:
        NSMException(
                const char*         aFileName,
                const unsigned int  aLineNumber,
                const char*         aFunctionName,
                const std::string&  aErrorMessage);
};

class InvalidArgumentException : public BaseException
{
	public:
		InvalidArgumentException(
                const char*         aFileName, 
                const unsigned int  aLineNumber, 
                const char*         aFunctionName, 
                const std::string&  aErrorMessage);
};

class SwitchException : public BaseException
{
	public:
		SwitchException(
                const char*         aFileName, 
                const unsigned int  aLineNumber, 
                const char*         aFunctionName,
                const std::string&  aErrorMessage);
};

class FileException : public BaseException
{
	public:
		FileException(
                const char*         aFileName, 
                const unsigned int  aLineNumber, 
                const char*         aFunctionName, 
                const char*         aErrorFileName, 
                const std::string&  aErrorMessage); 
};
