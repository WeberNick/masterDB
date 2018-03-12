#pragma once

#include "types.hh"
#include "error.hh"
#include "constants.hh"

#include <semaphore.h>
#include <cerrno>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" /* ignore deprecated compiler warnings */
class Semaphore
{
    public:
        explicit Semaphore(const int aShared, const uint aValue) : _semaphore()
        { 
            if(sem_init(&_semaphore, aShared, aValue) == -1 && Constants::getInstance()->trace())
            {
                printError("'sem_init' call failed", errno);
            }
        }
        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;
        ~Semaphore()
        { 
            if(sem_destroy(&_semaphore) == -1 && Constants::getInstance()->trace())
            {
                printError("'sem_destroy' call failed", errno);
            }
        }

    public:
        inline void wait()
        {
            int lReturnCode;
            do
            {
                lReturnCode = sem_wait(&_semaphore);
            } while (lReturnCode == -1 && errno == EINTR);
            if(lReturnCode == -1 && Constants::getInstance()->trace())
            {
                printError("'sem_wait' call failed", errno);
            }
        }

        inline void try_wait()
        {
            if(sem_trywait(&_semaphore) == -1 && Constants::getInstance()->trace())
            {
                printError("'sem_trywait' call failed", errno);
            }
        }

        inline void unlock()
        { 
            if(sem_post(&_semaphore) == -1 && Constants::getInstance()->trace())
            {
                printError("'sem_post' call failed", errno);
            }
        }

    private:
        sem_t _semaphore;
};
#pragma GCC diagnostic pop
