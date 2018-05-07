#include "tuples.hh"

Partition_T::Partition_T() : 
    _size(0), _pID(0), _pName(""), _pPath(""), _pType(0), _pGrowth(0)
{}


Partition_T::Partition_T(const uint32_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth) : 
    _size(0), _pID(aPID), _pName(aName), _pPath(aPath), _pType(aType), _pGrowth(aGrowth)
{
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(aType) + sizeof(aGrowth); //+1 for each string for \0
    std::cout << "Size of class: " << _size << std::endl;
}
void Partition_T::init(const uint32_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth)
{
    _pID = aPID;
    _pName = aName;
    _pPath = aPath;
    _pType = aType;
    _pGrowth = aGrowth;
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(_pType) + sizeof(_pGrowth); //+1 for each string for \0
}

void Partition_T::toDisk(byte* aPtr)
{
    size_t test = 0;
    *(uint32_t*)aPtr = _pID;
    aPtr += sizeof(_pID);
    test += sizeof(_pID);
    for(size_t i = 0; i < _pName.size() + 1; ++i)
    {
        *(char*)aPtr = _pName.c_str()[i];
        std::bitset<8> b(*(char*)aPtr);
        std::cout << b << "(" << (*(char*)aPtr) << ") ";
        ++aPtr;
        ++test;
    }
    std::cout << std::endl;

    for(size_t i = 0; i < _pPath.size() + 1; ++i)
    {
        *(char*)aPtr = _pPath.c_str()[i];
        std::bitset<8> b(*(char*)aPtr);
        std::cout << b << "(" <<(*(char*)aPtr) << ") ";
        ++aPtr;
        ++test;
    }
    std::cout << std::endl;    

    *(uint8_t*)aPtr = _pType;
    std::cout << "##### Type: " << _pType << std::endl;
    uint t = _pType;
    std::cout << "#### as uint: " << t << std::endl;
    ++aPtr;
    ++test;

    *(uint16_t*)aPtr = _pGrowth;
    test += sizeof(_pGrowth);
    std::cout << "range: " << test << std::endl;
}


void Partition_T::toMemory(byte* aPtr)
{
    _pID = *(uint32_t*)aPtr;
    aPtr += sizeof(_pID);
    _pName = std::string((const char*)aPtr);
    aPtr += _pName.size() + 1;
    _pPath = std::string((const char*)aPtr);
    aPtr += _pPath.size() + 1;
    _pType = *(uint8_t*)aPtr;
    ++aPtr;
    _pGrowth = *(uint16_t*)aPtr;
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(_pType) + sizeof(_pGrowth); //+1 for each string for \0
}


std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple)
{
    stream << "Size: " << aPartTuple.size()
        << ", ID: " << aPartTuple.ID()
        << ", Name: " << aPartTuple.name()
        << ", Path: " << aPartTuple.path()
        << ", Type: " << aPartTuple.type()
        << ", Growth: " << aPartTuple.growth();
    return stream;
}
