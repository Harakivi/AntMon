#pragma once
#include "uart.h"
#include <stdarg.h>
#include <stdint.h>
#include "printf.h"

namespace InternalPeriph
{
    class iUart
    {
    public:
        virtual bool Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t) = nullptr) = 0;

        virtual bool Write(uint8_t *buffer, uint32_t count) = 0;

        virtual int print(const char *format, ...) = 0;

        virtual uint32_t GetReceived(uint8_t *buffer, uint32_t size) = 0;
    };

    template <int uart>
    class Uart : public iUart
    {
    private:
        bool _opened;
        Uart() : _opened(false) {}

    public:
        static Uart *Get()
        {
            static Uart instance = Uart();
            return &instance;
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

        bool Write(uint8_t *buffer, uint32_t count)
        {
            if (_opened)
            {
                UartWrite(uart, buffer, count);
                return true;
            }
            return false;
        }

        int print(const char *format, ...)
        {
            uint32_t writted = 0;
            if (_opened)
            {
                va_list va;
                va_start(va, format);
                char buffer[TX_BUFF_SIZE];
                writted = vsnprintf(buffer, TX_BUFF_SIZE, format, va);
                va_end(va);
                UartWrite(uart, (uint8_t *)buffer, writted);
            }
            return writted;
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