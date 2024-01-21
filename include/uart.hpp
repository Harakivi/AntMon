#pragma once
#include "uart.h"
#include <stdint.h>


namespace InternalPeriph
{
    class UartHandler
    {
    public:
        virtual void onByteReceived(uint8_t data) = 0;
    };

    class iUart
    {
    public:
        virtual bool Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t) = nullptr) = 0;

        virtual bool Open(uint32_t BaudRate, UartHandler *handler) = 0;

        virtual bool Write(uint8_t *buffer, uint32_t count) = 0;

        virtual uint32_t GetReceived(uint8_t *buffer, uint32_t size) = 0;
    };

    template <int uart>
    class Uart : public iUart
    {
    private:
        bool _opened;
        UartHandler *_handler;
        Uart() : _opened(false), _handler(nullptr) {}

        static void _onByteReceived(uint8_t data);

    public:
        static Uart &Get()
        {
            static Uart instance = Uart();
            return instance;
        }

        bool Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t) = nullptr)
        {
            bool res = false;
            if constexpr (uart == 1)
            {
                res = Uart1Open(BaudRate, onByteReceived);
            }
            if constexpr (uart == 2)
            {
                res = Uart2Open(BaudRate, onByteReceived);
            }
            if constexpr (uart == 3)
            {
                res = Uart3Open(BaudRate, onByteReceived);
            }
            if (res)
            {
                _opened = true;
            }
            return res;
        }

        bool Open(uint32_t BaudRate, UartHandler *handler)
        {
            _handler = handler;
            return Open(BaudRate, _onByteReceived);
        }
        bool Write(uint8_t *buffer, uint32_t count)
        {
            if (_opened)
            {
                UartWrite(uart, buffer, count);
                return true;
            }
            return false;
        }

        uint32_t GetReceived(uint8_t *buffer, uint32_t size)
        {
            if (_opened)
            {
                return UartGetReceived(uart, buffer, size);
            }
            return 0;
        }
    };
}

template <int uart>
void InternalPeriph::Uart<uart>::_onByteReceived(uint8_t data)
{
    if (Uart<uart>::Get()._handler != nullptr)
        Uart<uart>::Get()._handler->onByteReceived(data);
}