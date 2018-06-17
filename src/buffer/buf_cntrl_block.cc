#include "buf_cntrl_block.hh"

const CB* BufferControlBlock::_cb = nullptr;

void BufferControlBlock::setCB(const CB* aControlBlock) noexcept
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

void BufferControlBlock::upgradeLock(LOCK_MODE aMode) noexcept
{
    if(getLockMode() <= aMode) //is upgrade needed?
    {
        switch(aMode)
        {
            case kNOLOCK:
                setLockMode(aMode);
                break;
            case kSHARED:
                getMtx().lock_shared();
                setLockMode(aMode);  
                incrFixCount();
                break;
            case kEXCLUSIVE:
                if(getLockMode() == kSHARED)
                {
                    getMtx().unlock_shared();
                    decrFixCount();
                }
                getMtx().lock();
                setLockMode(aMode);
                setFixCount(1);
                break;
            default:
                const std::string lErrMsg("Lock type not supported");
                TRACE(lErrMsg);
                throw SwitchException(FLF, lErrMsg);
                break;
        }
    }
}
