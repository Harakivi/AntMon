#pragma once
#include "uart.hpp"
#include <stdarg.h>
#include <stdint.h>
#include "smallprintf.h"
#include "cmd_parser.h"

namespace Drivers
{
    class Cli: public InternalPeriph::UartHandler
    {
    private:
        Cli(){}
        InternalPeriph::iUart *_uart;
        uint8_t cli_header[256];
        uint8_t cli_header_len;
        uint8_t cli_echo[100];
        uint8_t cli_echo_len;
        bool needToParse;
        bool needToUpdateCli;
    public:
        Cli(InternalPeriph::iUart *uart);
        void Open(uint32_t BaudRate);
        void Loop(uint32_t time);
        void AddCmd(cmd_t cmd);
        int print(const char *format, ...);
        int printHeader(const char *format, ...);
        void clearHeader();
        void onByteReceived(uint8_t data);
    };
}