#include "buf_cntrl_block.hh"

const CB* BufferControlBlock::_cb = nullptr;

void BufferControlBlock::setCB(const CB* aControlBlock)
{
    _cb = aControlBlock;
}

BufferControlBlock::BufferControlBlock() :
    _pageID(),
    _frameIndex(-1),
    _pageMtx(),
    _mode(kNoType),
    _modified(false),
    _fixCount(0),
    _nextInChain(nullptr)
{}

BufferControlBlock::~BufferControlBlock()
{

}

void BufferControlBlock::upgradeLock(LOCK_MODE aMode)
{
    if(getLockMode() < aMode) //is upgrade needed?
    {
        switch(aMode)
        {
            case kNOLOCK:
                setLockMode(aMode);
                break;
            case kSHARED:
                getMtx().lock_shared();
                setLockMode(aMode);  
                break;
            case kEXCLUSIVE:
                getMtx().lock();
                setLockMode(aMode);
                setFixCount(1);
                setModified(true);
                break;
            default:
                const std::string lErrMsg("Lock type not supported");
                if(_cb->trace()){ Trace::getInstance().log(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg); }
                throw SwitchException(__FILE__, __LINE__, __PRETTY_FUNCTION__, lErrMsg);
                break;
        }
    }
}
