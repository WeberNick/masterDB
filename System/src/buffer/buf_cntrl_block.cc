#include "buf_cntrl_block.hh"

BufferControlBlock::BufferControlBlock() :
    _pageID(),
    _frameIndex(),
    _pageSem(),
    _modified(),
    _fixCount(),
    _prevInLRU(),
    _nextInLRU(),
    _nextInChain()
{}

BufferControlBlock::~BufferControlBlock()
{

}


