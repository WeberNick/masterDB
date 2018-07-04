#include "employee_t.hh"

uint64_t Employee_T::_idCount = 1;

/**
 * @brief Construct a new Employee_T::Employee_T object
 * 
 */
Employee_T::Employee_T() : 
    Tuple(0),
    _id(0),
    _salary(0.0),
    _age(0),
    _name("")
{
    TRACE("'Employee_T' object was default constructed: " + to_string());
}

/**
 * @brief Construct a new Employee_T::Employee_T object
 * 
 * @param aName the name
 * @param aSalary the salary
 * @param aAge the age
 */
Employee_T::Employee_T(const std::string& aName, const double aSalary, const uint8_t aAge) : 
    Tuple(0),
    _id(_idCount++),
    _salary(aSalary),
    _age(aAge),
    _name(aName)
{
    _size = sizeof(_id) + sizeof(_salary) + sizeof(_age) + (_name.size() + 1) ; //+1 for each string for \0
    TRACE("'Employee_T' object was constructed: " + to_string());
}

/**
 * @brief Copy-Construct a new Employee_T::Employee_T object
 * 
 * @param aEmployeeTuple the object to copy
 */
Employee_T::Employee_T(const Employee_T& aEmployeeTuple) :
    Tuple(aEmployeeTuple.size()),
    _id(aEmployeeTuple.ID()),
    _salary(aEmployeeTuple.salary()),
    _age(aEmployeeTuple.age()),
    _name(aEmployeeTuple.name())
{
    TRACE("'Employee_T' object was copy-constructed: " + to_string());
}

Employee_T& Employee_T::operator=(const Employee_T& aEmployeeTuple)
{
    if(this != &aEmployeeTuple)
    {
        //exception safe copy assignment with swap would be overkill
        _size    = aEmployeeTuple.size();
        _id      = aEmployeeTuple.ID();
        _salary  = aEmployeeTuple.salary();
        _age     = aEmployeeTuple.age();
        _name    = aEmployeeTuple.name();
    }
    return *this;
}

void Employee_T::init(const std::string& aName, const double aSalary, const uint8_t aAge) noexcept
{
    _id = _idCount++;
    _salary = aSalary;
    _age = aAge;
    _name = aName;
    _size = sizeof(_id) + sizeof(_salary) + sizeof(_age) + (_name.size() + 1) ; //+1 for each string for \0
    TRACE("'Employee_T' object was initialized: " + to_string());
}

void Employee_T::toDisk(byte* aPtr) const noexcept
{
    *(uint64_t*)aPtr = _id;
    aPtr += sizeof(_id);

    *(double*)aPtr = _salary;
    aPtr += sizeof(_salary);

    *(uint8_t*)aPtr = _age;
    aPtr += sizeof(_age);

    for(size_t i = 0; i < _name.size() + 1; ++i)
    {
        *(char*)aPtr = _name.c_str()[i];
        ++aPtr;
    }
    TRACE("Transformed 'Employee_T' object (" + to_string() + ") to its disk representation");
}

void Employee_T::toDisk(byte* aPtr) noexcept
{
    static_cast<const Employee_T&>(*this).toDisk(aPtr);
}


void Employee_T::toMemory(byte* aPtr) noexcept
{
    _id = *(uint64_t*)aPtr;
    aPtr += sizeof(_id);

    _salary = *(double*)aPtr;
    aPtr += sizeof(_salary);

    _age = *(uint8_t*)aPtr;
    aPtr += sizeof(_age);

    _name = std::string((const char*)aPtr);
    aPtr += _name.size() + 1;
    _size = sizeof(_id) + sizeof(_salary) + sizeof(_age) + (_name.size() + 1) ; //+1 for each string for \0
    TRACE("Transformed 'Employee_T' object (" + to_string() + ") to its memory representation");
}

std::ostream& operator<< (std::ostream& stream, const Employee_T& aEmployeeTuple)
{
    stream << aEmployeeTuple.to_string();
    return stream;
}
