#pragma once
#include <stdint.h>
#include "wdt.h"

namespace InternalPeriph
{
    class iWdt
    {
    public:
        virtual void ResetCounter() = 0;
    };

    class Wdt : public iWdt
    {
    private:
        bool _started;
        Wdt() : _started(false) {}

    public:
        static Wdt &Get()
        {
            static Wdt instance = Wdt();
            return instance;
        }

        bool Start(uint32_t period_us)
        {
            return WdtStart(period_us);
        }

        virtual void ResetCounter()
        {
            WdtResetCounter();
        }
    };
}