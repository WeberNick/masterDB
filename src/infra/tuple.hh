/*********************************************************************
 * @file    tuple.hh
 * @author 	Nick Weber
 * @date    Jun 30, 2018
 * @brief 	Abstract class providing an interface for all tuple types
 * @bugs 	TBD
 * @todos 	TBD
 * 
 * @section	DESCRIPTION
 * TBD
 * 
 * @section USE
 * TBD
 ********************************************************************/
#pragma once

#include "types.hh"
#include "trace.hh"
#include "exception.hh"

class Tuple
{
    public:
        Tuple(const size_t aSize) : _size(aSize){}
        explicit Tuple(const Tuple&) = default;
        explicit Tuple(Tuple&&) = default;
        Tuple& operator=(const Tuple&) = default;
        Tuple& operator=(Tuple&&) = default;
        virtual ~Tuple() = default;
    
    public:
        virtual void toDisk(byte* aPtr) const noexcept = 0;
        virtual void toDisk(byte* aPtr) noexcept = 0;
        virtual void toMemory(byte* aPtr) noexcept = 0;
    
    public:
        // Getter
        inline size_t size() const noexcept { return _size; };
        inline size_t size() noexcept { return _size; }
        virtual std::string to_string() const noexcept = 0;
        virtual std::string to_string() noexcept = 0;

    protected:
        size_t _size; // size of class
};

