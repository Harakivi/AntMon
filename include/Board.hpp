#pragma once
#include "uart.hpp"
#include "dac.hpp"
#include "wdt.hpp"
#include "STM32Bootloader.h"
#include "DeviceClass.hpp"

namespace Hardware
{
    class AntBmsMonitor
    {
    private:
        AntBmsMonitor() {}
        typedef InternalPeriph::Wdt Wdt_t;
        typedef InternalPeriph::Uart<1> cliUart_t;
        typedef InternalPeriph::Uart<2> bmsUart_t;
        typedef InternalPeriph::Dac<1> indicatorDac_t;

    public:
        static InternalPeriph::iWdt &wdt;
        static InternalPeriph::iUart &cliUart;
        static InternalPeriph::iUart &bmsUart;
        static InternalPeriph::iDac &indicatorDac;
        static void Init();
    };
};
