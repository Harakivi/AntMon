#pragma once
#include <stdint.h>
#include "dac.h"

namespace InternalPeriph
{
    class Dac
    {
    private:
        Dac():_chanInitied{false} {}
        bool _chanInitied[2];
    public:
        static Dac* Get()
        {
            static Dac instance = Dac();
            static bool _initied = DacInit(); 
            if(!_initied)
            {
                return nullptr;
            }
            return &instance;
        }

        bool ChannelInit(uint8_t channel = 1)
        {
            _chanInitied[channel - 1] = DacChannelInit(channel);
            return _chanInitied[channel - 1];
        }

        bool SetValue(uint16_t value, uint8_t channel = 1)
        {
            if(!_chanInitied[channel - 1])
            {
                return false;
            }
            DacSetValue(channel, value);
            return true;
        }

        uint16_t GetValue(uint8_t channel = 1)
        {
            if(!_chanInitied[channel - 1])
            {
                return 0;
            }
            return DacGetValue(channel);
        }
    };
}