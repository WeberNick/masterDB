#include "partition_t.hh"

/**
 * @brief Construct a new Partition_T::Partition_T object
 * 
 */
Partition_T::Partition_T() : 
    Tuple(0),
    _pID(0),
    _pName(""),
    _pPath(""),
    _pType(0),
    _pGrowth(0)
{
    TRACE("'Partition_T' object was default-constructed: " + to_string());
}

/**
 * @brief Construct a new Partition_T::Partition_T object
 * 
 * @param aPID 
 * @param aName 
 * @param aPath 
 * @param aType 
 * @param aGrowth 
 */
Partition_T::Partition_T(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth) : 
    Tuple(0),
    _pID(aPID),
    _pName(aName),
    _pPath(aPath),
    _pType(aType),
    _pGrowth(aGrowth)
{
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(_pType) + sizeof(_pGrowth); // +1 for each string for \0
    TRACE("'Partition_T' object was constructed: " + to_string());
}

/**
 * @brief Construct a new Partition_T::Partition_T object
 * 
 * @param aPartitionTuple 
 */
Partition_T::Partition_T(const Partition_T& aPartitionTuple) :
    Tuple(aPartitionTuple.size()),
    _pID(aPartitionTuple.ID()),
    _pName(aPartitionTuple.name()),
    _pPath(aPartitionTuple.path()),
    _pType(aPartitionTuple.type()),
    _pGrowth(aPartitionTuple.growth())
{
    TRACE("'Partition_T' object was copy-constructed: " + to_string());
}

Partition_T& Partition_T::operator=(const Partition_T& aPartitionTuple)
{
    if(this != &aPartitionTuple)
    {
        //exception safe copy assignment with swap would be overkill
        _size    = aPartitionTuple.size();
        _pID     = aPartitionTuple.ID();
        _pName   = aPartitionTuple.name();
        _pPath   = aPartitionTuple.path();
        _pType   = aPartitionTuple.type();
        _pGrowth = aPartitionTuple.growth();
    }
    return *this;
}

void Partition_T::init(const uint8_t aPID, const std::string& aName, const std::string& aPath, const uint8_t aType, const uint16_t aGrowth) noexcept
{
    _pID = aPID;
    _pName = aName;
    _pPath = aPath;
    _pType = aType;
    _pGrowth = aGrowth;
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(_pType) + sizeof(_pGrowth); //+1 for each string for \0
    TRACE("'Partition_T' object was initialized: " + to_string());
}

void Partition_T::toDisk(byte* aPtr) const noexcept
{
    *(uint8_t*)aPtr = _pID;
    aPtr += sizeof(_pID);
    for(size_t i = 0; i < _pName.size() + 1; ++i)
    {
        *(char*)aPtr = _pName.c_str()[i];
        ++aPtr;
    }

    for(size_t i = 0; i < _pPath.size() + 1; ++i)
    {
        *(char*)aPtr = _pPath.c_str()[i];
        ++aPtr;
    }
    *(uint8_t*)aPtr = _pType;
    aPtr += sizeof(_pType);
    *(uint16_t*)aPtr = _pGrowth;
    aPtr += sizeof(_pGrowth);
    TRACE("Transformed 'Partition_T' object (" + to_string() + ") to its disk representation");
}

void Partition_T::toDisk(byte* aPtr) noexcept
{
    static_cast<const Partition_T&>(*this).toDisk(aPtr);
}

void Partition_T::toMemory(byte* aPtr) noexcept
{
    _pID = *(uint8_t*)aPtr;
    aPtr += sizeof(_pID);
    _pName = std::string((const char*)aPtr);
    aPtr += _pName.size() + 1;
    _pPath = std::string((const char*)aPtr);
    aPtr += _pPath.size() + 1;
    _pType = *(uint8_t*)aPtr;
    aPtr += sizeof(_pType);
    _pGrowth = *(uint16_t*)aPtr;
    aPtr += sizeof(_pGrowth);
    _size = sizeof(_pID) + (_pName.size() + 1) + (_pPath.size() + 1) + sizeof(_pType) + sizeof(_pGrowth); //+1 for each string for \0
    TRACE("Transformed 'Partition_T' object (" + to_string() + ") to its memory representation");
}

std::ostream& operator<< (std::ostream& stream, const Partition_T& aPartTuple)
{
    stream << aPartTuple.to_string();
    return stream;
}
