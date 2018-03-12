#include "buf_cntrl_block.hh"

BufferControlBlock::BufferControlBlock() :
    _pageID(),
    _frameIndex(0),
    _pageSem(0,0),
    _modified(false),
    _fixCount(0),
    _prevInLRU(nullptr),
    _nextInLRU(nullptr),
    _nextInChain(nullptr)
{}

BufferControlBlock::~BufferControlBlock()
{

}


