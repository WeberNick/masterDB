#include "buf_acc_cntrl_block.hh"

BufferAccessControlBlock::BufferAccessControlBlock() :
    _pageID(),
    _pagePtr(nullptr),
    _pageSem(nullptr),
    _modified(false),
    _invalid(false)
{}

BufferAccessControlBlock::~BufferAccessControlBlock()
{

}
