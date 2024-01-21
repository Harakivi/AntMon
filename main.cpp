#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "Board.hpp"
#include "Cli.hpp"
#include "AntBmsDriver.hpp"

typedef Hardware::AntBmsMonitor Board;

extern "C" void Reset_Handler();

int main()
{
    Board::Init();

    static Drivers::Cli cli = Drivers::Cli(Board::cliUart);
    cli.Open(115200);

    static auto printVersion = []()
    {
        cli.print("\r\nVersion = %s", VERSION);
        cli.print("\r\nDate = %s", DATE);
    };

    cli.AddCmd(cmd_t{"version", NULL, printVersion, NULL});
    cli.AddCmd(cmd_t{"bootloader", NULL, EnterBootloader, NULL});
    cli.AddCmd(cmd_t{"reset", NULL, Reset_Handler, NULL});

    static Drivers::AntBmsDriver ant = Drivers::AntBmsDriver(2000, Board::bmsUart);
    static DeviceClass::AntBmsMonitor device = DeviceClass::AntBmsMonitor(ant, cli, Board::indicatorDac);

    while (1)
    {
        ant.Loop(HAL_GetTick());
        cli.Loop(HAL_GetTick());
        device.Loop(HAL_GetTick());
        Board::wdt.ResetCounter();
    }
}