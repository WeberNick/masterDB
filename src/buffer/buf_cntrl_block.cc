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


void BufferControlBlock::lock(LOCK_MODE aMode) noexcept
{
    TRACE("Trying to aquire '" + lockModeToString(aMode) + "' lock for BCB with PID : " + _pageID.to_string());
    switch(aMode)
    {
        case kNOLOCK:
            setLockMode(aMode);
            incrFixCount();
            break;
        case kSHARED:
            getMtx().lock_shared();
            setLockMode(aMode);  
            incrFixCount();
            break;
        case kEXCLUSIVE:
            getMtx().lock();
            setLockMode(aMode);
            setFixCount(1);
            break;
        default:
            const std::string lErrMsg("Lock type not supported");
            TRACE(lErrMsg);
            ASSERT_MSG("Invalid default-case of switch statement reached");
            break;
    }
    TRACE("Lock '" + lockModeToString(aMode) + "' for BCB with PID : " + _pageID.to_string() + " aquired");
}

void BufferControlBlock::lock() noexcept
{
    lock(kEXCLUSIVE);
}

void BufferControlBlock::lock_shared() noexcept
{
    lock(kSHARED);
}


void BufferControlBlock::unlock() noexcept
{
    TRACE("Unlock BCB with '" + lockModeToString(getLockMode()) + "' lock and with PID : " + _pageID.to_string());
    switch(getLockMode())
    {
        case kNOLOCK:
            break;
        case kSHARED:
            getMtx().unlock_shared();
            break;
        case kEXCLUSIVE:
            getMtx().unlock();
            break;
        default:
            const std::string lErrMsg("Lock type not supported");
            TRACE(lErrMsg);
            ASSERT_MSG("Invalid default-case of switch statement reached");
            break;
    }
    decrFixCount();
    TRACE("BCB with lock '" + lockModeToString(getLockMode()) + "' and with PID : " + _pageID.to_string() + " unlocked");
}

void BufferControlBlock::upgradeLock(LOCK_MODE aMode) noexcept
{
    TRACE("Trying to upgrade lock from '" + lockModeToString(getLockMode()) + "' to '" + lockModeToString(aMode) + "' for BCB with PID : " + _pageID.to_string());
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
                ASSERT_MSG("Invalid default-case of switch statement reached");
                break;
        }
    }
    TRACE("Successfully upgraded lock for BCB with PID : " + _pageID.to_string());
}
