#include "segment_t.hh"

/**
 * @brief Construct a new Segment_T::Segment_T object
 * 
 */
Segment_T::Segment_T() : 
    Tuple(0),
    _sPID(0),
    _sID(0),
    _sName(""),
    _sType(0),
    _sFirstPage(0)
{
    TRACE("'Segment_T' object was default-constructed: " + to_string());
}

/**
 * @brief Construct a new Segment_T::Segment_T object
 * 
 * @param aPID 
 * @param aSID 
 * @param aName 
 * @param aType 
 * @param aFirstPage 
 */
Segment_T::Segment_T(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage) : 
    Tuple(0),
    _sPID(aPID),
    _sID(aSID),
    _sName(aName),
    _sType(aType),
    _sFirstPage(aFirstPage)
{
    _size = sizeof(_sPID) + sizeof(_sID) + (_sName.size() + 1) + sizeof(_sType) + sizeof(_sFirstPage); // +1 for each string for \0
    TRACE("'Segment_T' object was constructed: " + to_string());
}

/**
 * @brief Construct a new Segment_T::Segment_T object
 * 
 * @param aSegmentTuple 
 */
Segment_T::Segment_T(const Segment_T& aSegmentTuple) :
    Tuple(aSegmentTuple.size()),
    _sPID(aSegmentTuple.partID()),
    _sID(aSegmentTuple.ID()),
    _sName(aSegmentTuple.name()),
    _sType(aSegmentTuple.type()),
    _sFirstPage(aSegmentTuple.firstPage())
{
    TRACE("'Segment_T' object was copy-constructed: " + to_string());
}

Segment_T& Segment_T::operator=(const Segment_T& aSegmentTuple)
{
    if(this != &aSegmentTuple)
    {
        // exception safe copy assignment with swap would be overkill
        _size       = aSegmentTuple.size();
        _sPID       = aSegmentTuple.partID();
        _sID        = aSegmentTuple.ID();
        _sName      = aSegmentTuple.name();
        _sType      = aSegmentTuple.type();
        _sFirstPage = aSegmentTuple.firstPage();
    }
    return *this;
}

void Segment_T::init(const uint8_t aPID, const uint16_t aSID, const std::string& aName, const uint8_t aType, const uint32_t aFirstPage) noexcept
{
    _sPID = aPID;
    _sID = aSID;
    _sName = aName;
    _sType = aType;
    _sFirstPage = aFirstPage;
    _size = sizeof(_sPID) + sizeof(_sID) + (_sName.size() + 1) + sizeof(_sType) + sizeof(_sFirstPage); // +1 for each string for \0
    TRACE("'Segment_T' object was initialized: " + to_string());
}

void Segment_T::toDisk(byte* aPtr) const noexcept
{
    *(uint8_t*)aPtr = _sPID;
    aPtr += sizeof(_sPID);
    *(uint16_t*)aPtr = _sID;
    aPtr += sizeof(_sID);
    for(size_t i = 0; i < _sName.size() + 1; ++i)
    {
        *(char*)aPtr = _sName.c_str()[i];
        ++aPtr;
    }

    *(uint8_t*)aPtr = _sType;
    aPtr += sizeof(_sType);
    *(uint32_t*)aPtr = _sFirstPage;
    aPtr += sizeof(_sFirstPage);
    TRACE("Transformed 'Segment_T' object (" + to_string() + ") to its disk representation");
}

void Segment_T::toDisk(byte* aPtr) noexcept
{
    static_cast<const Segment_T&>(*this).toDisk(aPtr);
}

void Segment_T::toMemory(byte* aPtr) noexcept
{
    _sPID = *(uint8_t*)aPtr;
    aPtr += sizeof(_sPID);
    _sID = *(uint16_t*)aPtr;
    aPtr += sizeof(_sID);
    _sName = std::string((const char*)aPtr);
    aPtr += _sName.size() + 1;
    _sType = *(uint8_t*)aPtr;
    aPtr += sizeof(_sType);
    _sFirstPage = *(uint32_t*)aPtr;
    _size = sizeof(_sPID) + sizeof(_sID) + (_sName.size() + 1) + sizeof(_sType) + sizeof(_sFirstPage); //+1 for each string for \0
    TRACE("Transformed 'Segment_T' object (" + to_string() + ") to its memory representation");
}

std::ostream& operator<< (std::ostream& stream, const Segment_T& aSegmentTuple)
{
    stream << aSegmentTuple.to_string();
    return stream;
}
