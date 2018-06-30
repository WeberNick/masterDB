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
    _mode(LOCK_MODE::kNoType),
    _modified(false),
    _fixCount(0),
    _nextInChain(nullptr)
{}


void BufferControlBlock::lock(LOCK_MODE aMode) noexcept
{
    TRACE("Trying to aquire '" + lockModeToString(aMode) + "' lock for BCB with PID : " + _pageID.to_string());
    switch(aMode)
    {
        case LOCK_MODE::kNOLOCK:
            incrFixCount();
            if(toType(getLockMode()) > toType(LOCK_MODE::kNOLOCK)){ break; } //BCB has a "higher" lock
            else{ setLockMode(aMode); }
            break;
        case LOCK_MODE::kSHARED:
            getMtx().lock_shared();
            setLockMode(aMode);  
            incrFixCount();
            break;
        case LOCK_MODE::kEXCLUSIVE:
            getMtx().lock();
            setLockMode(aMode);
            setFixCount(1);
            break;
        default:
            TRACE("Lock type not supported");
            ASSERT_MSG("Invalid default-case of switch statement reached");
            break;
    }
    TRACE("Lock '" + lockModeToString(aMode) + "' for BCB with PID : " + _pageID.to_string() + " aquired");
}

void BufferControlBlock::lock() noexcept
{
    lock(LOCK_MODE::kEXCLUSIVE);
}

void BufferControlBlock::lock_shared() noexcept
{
    lock(LOCK_MODE::kSHARED);
}


void BufferControlBlock::unlock() noexcept
{
    TRACE("Unlock BCB with '" + lockModeToString(getLockMode()) + "' lock and with PID : " + _pageID.to_string());
    switch(getLockMode())
    {
        case LOCK_MODE::kNOLOCK:
            break;
        case LOCK_MODE::kSHARED:
            getMtx().unlock_shared();
            break;
        case LOCK_MODE::kEXCLUSIVE:
            getMtx().unlock();
            break;
        default:
            TRACE("Lock type not supported");
            ASSERT_MSG("Invalid default-case of switch statement reached");
            break;
    }
    decrFixCount();
    TRACE("BCB with lock '" + lockModeToString(getLockMode()) + "' and with PID : " + _pageID.to_string() + " unlocked");
}

void BufferControlBlock::upgradeLock(LOCK_MODE aMode) noexcept
{
    TRACE("Trying to upgrade lock from '" + lockModeToString(getLockMode()) + "' to '" + lockModeToString(aMode) + "' for BCB with PID : " + _pageID.to_string());
    if(toType(getLockMode()) < toType(aMode)) //is upgrade needed?
    {
        switch(aMode)
        {
            case LOCK_MODE::kNOLOCK:
                setLockMode(aMode);
                break;
            case LOCK_MODE::kSHARED:
                getMtx().lock_shared();
                setLockMode(aMode);  
                incrFixCount();
                break;
            case LOCK_MODE::kEXCLUSIVE:
                if(toType(getLockMode()) == toType(LOCK_MODE::kSHARED))
                {
                    getMtx().unlock_shared();
                    decrFixCount();
                }
                getMtx().lock();
                setLockMode(aMode);
                setFixCount(1);
                break;
            case LOCK_MODE::kNoType:
            case LOCK_MODE::kLockModeSize:
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
