/*********************************************************************
 * @file    segment_t.hh
 * @author 	Nick Weber
 * @date    Mai 07, 2018
 * @brief 	Tuple class for employees. Used for transforming to disk and memory representations
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

#include "tuple.hh"
#include "types.hh"
#include "exception.hh"
#include "trace.hh"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <utility>

class Employee_T : public Tuple
{
    public:
        Employee_T();
        Employee_T(const std::string& aName, const double aSalary, const uint8_t aAge); 
        Employee_T(const Employee_T& aEmployeeTuple);
        explicit Employee_T(Employee_T&&) = delete;
        Employee_T& operator=(const Employee_T& aEmployeeTuple);
        Employee_T& operator=(Employee_T&&) = delete;
        ~Employee_T() = default;

    public:
        void init(const std::string& aName, const double aSalary, const uint8_t aAge) noexcept;
        void toDisk(byte* aPtr) const noexcept override;
        void toDisk(byte* aPtr) noexcept override;
        void toMemory(byte* aPtr) noexcept override;
    
    public:
        //getter
        inline uint64_t ID() const noexcept { return _id; }
        inline uint64_t ID() noexcept { return _id; }
        inline uint8_t age() const noexcept { return _age; }
        inline uint8_t age() noexcept { return _age; }
        inline const std::string& name() const noexcept { return _name; }
        inline const std::string& name() noexcept { return _name; }
        inline double salary() const noexcept { return _salary; }
        inline double salary() noexcept { return _salary; }
        inline std::string to_string() const noexcept override; 
        inline std::string to_string() noexcept override;
        
    private:
        static uint64_t _idCount;
        /* content of the tuple */
        uint64_t    _id;
        double    _salary;
        uint8_t    _age;
        std::string _name;
        
};

std::string Employee_T::to_string() const noexcept 
{ 
    return std::string("Employee (ID, Name, Age, Salary) : ") 
        + std::to_string(_id) + std::string(", '") 
        + _name + std::string("', ") 
        + std::to_string(_age) + std::string(", ") 
        + std::to_string(_salary); 
}

std::string Employee_T::to_string() noexcept 
{ 
    return static_cast<const Employee_T&>(*this).to_string(); 
}

using emp_vt = std::vector<Employee_T>;
std::ostream& operator<< (std::ostream& stream, const Employee_T& aEmpTuple);
