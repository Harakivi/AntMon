#pragma once
#include <stdint.h>
#include "dac.h"

namespace InternalPeriph
{
    class iDac
    {
    public:
        virtual bool ChannelInit() = 0;

        virtual bool SetValue(uint16_t value) = 0;

        virtual uint16_t GetValue() = 0;
    };

    template <uint8_t channel>
    class Dac : public iDac
    {
    private:
        Dac() : _chanInitied{false} {}
        bool _chanInitied;
        static bool _initied;

    public:
        static Dac &Get()
        {
            static Dac instance = Dac();
            return instance;
        }

        static bool Init()
        {
            if (!_initied)
                _initied = DacInit();
            return _initied;
        }

        virtual bool ChannelInit()
        {
            _chanInitied = DacChannelInit(channel);
            return _chanInitied;
        }

        virtual bool SetValue(uint16_t value)
        {
            if (!_initied && !_chanInitied)
            {
                return false;
            }
            DacSetValue(channel, value);
            return true;
        }

        virtual uint16_t GetValue()
        {
            if (!_initied && !_chanInitied)
            {
                return 0;
            }
            return DacGetValue(channel);
        }
    };

    template <uint8_t channel>
    bool Dac<channel>::_initied = false;
}