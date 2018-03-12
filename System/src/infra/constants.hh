#pragma once

#include "error.hh"

#include <string>

class Constants
{
    private:
        explicit Constants(const bool aTrace) :
            _trace(aTrace)
        {
        
        }
        Constants(const Constants&) = delete;
        Constants(Constants&&) = delete;
        Constants& operator=(const Constants&) = delete;
        Constants& operator=(Constants&&) = delete;

        ~Constants()
        {
            delete _instance;
        }

    public:
        static void create(const bool aTrace)
        {
            if(_instance) printError("Can not create Singleton twice"); 
            else _instance = new Constants(aTrace);
        }

        static const Constants* const getInstance()
        {
            return _instance; 
        }

        static void destroy()
        {
            if(_instance) delete _instance;
            else printError("Can not destroy Singleton instance before creating it");
        }
    
    public:
        inline const bool trace() const { return _trace; }


    public:
        static Constants*   _instance;
        bool                _trace; 
};

Constants* Constants::_instance = nullptr;
