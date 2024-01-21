#include "Cli.hpp"
#include "stm32f1xx_hal.h"

using namespace Drivers;

Cli::Cli(InternalPeriph::iUart &uart) : _uart(uart), _headerUpdater(nullptr), cli_header_len(0),
                                        cli_echo_len(0), needToParse(false), needToUpdateCli(true)
{
    static Drivers::Cli &constCli = *this;

    static auto help = []()
    {
        uint8_t listPos = 0;
        while (cmd_list[listPos].cmdString && listPos < MAX_CMD)
        {
            constCli.print("\r\n%s", cmd_list[listPos++].cmdString);
        }
    };

    static auto clear = []()
    {
        constCli.setHeaderUpdater(nullptr);
        constCli.clearHeader();
        constCli.clear();
    };

    constCli.AddCmd(cmd_t{"clear", NULL, clear, NULL});
    constCli.AddCmd(cmd_t{"help", NULL, help, NULL});
}

void Cli::Open(uint32_t BaudRate)
{
    _uart.Open(BaudRate, this);
}

void Cli::setHeaderUpdater(void (*headerUpdater)())
{
    _headerUpdater = headerUpdater;
}

void Cli::Loop(uint32_t time)
{
    static uint32_t lastHeaderUpdateTime = 0;
    if (needToParse)
    {
        if (!parseCmd((char *)cli_echo))
        {
            print("\033[2K\r");
            print("Error \"%s\" not found, try \"help\"", (char *)cli_echo);
        }
        cli_echo_len = 0;
        cli_echo[cli_echo_len] = 0;
        needToParse = false;
        needToUpdateCli = true;
    }
    if (needToUpdateCli)
    {
        // print("\033c");
        print("\r\n");
        _uart.Write(cli_header, cli_header_len);
        print("\033[0m");
        print("Shell->");
        _uart.Write(cli_echo, cli_echo_len);
        needToUpdateCli = false;
    }
    if (_headerUpdater != nullptr && time - lastHeaderUpdateTime > 1000)
    {
        _headerUpdater();
        lastHeaderUpdateTime = time;
    }
}

void Cli::AddCmd(cmd_t cmd)
{
    addCmd(cmd);
}

int Cli::print(const char *format, ...)
{
    uint32_t writted = 0;
    va_list va;
    va_start(va, format);
    char buffer[TX_BUFF_SIZE];
    writted = vsnprintf(buffer, TX_BUFF_SIZE, format, va);
    va_end(va);
    _uart.Write((uint8_t *)buffer, writted);
    return writted;
}

int Cli::printHeader(const char *format, ...)
{
    uint32_t writted = 0;
    va_list va;
    va_start(va, format);
    writted = vsnprintf((char *)(cli_header + cli_header_len), 256 - cli_header_len, format, va);
    va_end(va);
    cli_header_len += writted;
    needToUpdateCli = true;
    return writted;
}

void Cli::clear()
{
    print("\033c");
}

void Cli::clearHeader()
{
    cli_header_len = 0;
}

void Cli::onByteReceived(uint8_t data)
{
    {
        if (data != '\n' && data != '\r' && data != 127)
        {
            cli_echo[cli_echo_len++] = data;
            if (cli_echo_len < 99)
            {
                cli_echo[cli_echo_len] = 0;
            }
            _uart.Write(&data, 1);
        }
        else if (data == '\r')
        {
            needToParse = true;
        }
        else if (data == 127 && cli_echo_len > 0)
        {
            cli_echo[--cli_echo_len] = 0;
            _uart.Write((uint8_t *)"\b \b", 3);
        }
    }
}