#pragma once
#include <stdint.h>
#include "Cli.hpp"
#include "AntBmsDriver.hpp"
#include "dac.hpp"

namespace DeviceClass
{
    class AntBmsMonitor
    {
        static const uint16_t dacTable[];
        Drivers::AntBmsDriver &_bmsDriver;
        Drivers::Cli &_cli;
        InternalPeriph::iDac &_dac;
        bool _chargeBlink;
    public:
        AntBmsMonitor(Drivers::AntBmsDriver &bmsDriver, Drivers::Cli &cli, InternalPeriph::iDac &dac);
        void Loop(uint32_t time);
    };
}
