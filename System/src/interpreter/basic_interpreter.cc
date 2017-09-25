#include "basic_interpreter.cc"

//blocksize is fixed to 4096 bytes, headersize to 16 bytes. If it changes, it has to be changed here
BasicInterpreter::BasicInterpreter() : _pp(NULL), _blocksize(4096), _headersize(16)
{}

BasicInterpreter::~BasicInterpreter(){}

void BasicInterpreter::init(byte* aPP, uint64_t aLSN, uint32_t aOffset, uint8_t aPID)
{
    aPP+=_pagesize-_headersize;
    struct header_Block temp;
    temp._LSN=aLSN;
    temp._Offset=aOffset;
    temp._PID=aPID;
    temp.padding1=0;
    temp.padding2=0;
    temp.padding3=0;
    memcpy (aPP, &temp,_headersize);
}
