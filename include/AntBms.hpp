#pragma once
#include "uart.hpp"
#include <stdint.h>

namespace Drivers
{
    class AntBms
    {
    private:
    InternalPeriph::iUart* _uart; 
    public:
    AntBms(InternalPeriph::iUart* uart);
    bool UpdateLiveData();
    };
}