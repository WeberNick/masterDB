#include "buf_acc_cntrl_block.hh"

BufferAccessControlBlock::BufferAccessControlBlock() :
    _pageID(),
    _pagePtr(),
    _pageSem(),
    _modified(),
    _invalid()
{}

BufferAccessControlBlock::~BufferAccessControlBlock()
{

}
