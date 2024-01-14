#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "uart.hpp"
#include "Cli.hpp"
#include "AntBms.hpp"
#include "dac.hpp"
#include "wdt.hpp"
#include "STM32Bootloader.h"

enum TestModes
{
    None = 0,
    DacTestMode,
    SOCTestMode,
};

extern "C" void SystemClock_Config();

static const uint16_t dacTable[] = {2400, 2750, 2925, 3075, 3350};

typedef InternalPeriph::Wdt Wdt;
typedef InternalPeriph::Uart<1> cliUart;
typedef InternalPeriph::Uart<2> bmsUart;
typedef InternalPeriph::Dac Dac;
Dac *dac = Dac::Get();

typedef Drivers::Cli Cli;
#ifdef DEBUG
Cli cli = Cli(cliUart::Get());
TestModes testMode = None;
#endif

bool valid = true;
Drivers::AntLiveData livedata = {0};
uint16_t dacValCh1 = 0;
uint32_t liveDataReadLastTime = 0;

#ifdef DEBUG
uint8_t test_SOC = 0;

void printSOC()
{
    cli.clear();
    cli.clearHeader();
    if (livedata.Struct.SOC >= 75)
        cli.printHeader("\033[92m");
    else if (livedata.Struct.SOC < 75 && livedata.Struct.SOC >= 35)
        cli.printHeader("\033[93m");
    else if (livedata.Struct.SOC < 35)
        cli.printHeader("\033[91m");
    cli.printHeader("========================\r\n");
    cli.printHeader("AntBms SOC = %d\r\n", livedata.Struct.SOC);
    cli.printHeader("Total Voltage = %d\r\n", (livedata.Struct.TotalVoltage[0] << 8) + livedata.Struct.TotalVoltage[1]);
    cli.printHeader("DAC ch1 Value = %d\r\n", dacValCh1);
    cli.printHeader("Valid= %d\r\n", valid);
    cli.printHeader("========================\r\n");
}

void stateSOC()
{
    cli.setHeaderUpdater(printSOC);
}

void dacSetVal(uint16_t value)
{
    dac->SetValue(value, 1);
}

void socSetVal(uint16_t value)
{
    test_SOC = value;
}

void printDAC()
{
    cli.clear();
    cli.clearHeader();
    cli.printHeader("========================\r\n");
    cli.printHeader("DAC ch1 Value = %d\r\n", dac->GetValue(1));
    cli.printHeader("test_SOC = %d\r\n", test_SOC);
    cli.printHeader("========================\r\n");
}

void stateDacTestMode()
{
    testMode = DacTestMode;
    cli.setHeaderUpdater(printDAC);
}

void stateSOCTestMode()
{
    testMode = SOCTestMode;
    cli.setHeaderUpdater(printDAC);
}

void clearHeader()
{
    testMode = None;
    cli.setHeaderUpdater(nullptr);
    cli.clearHeader();
    cli.clear();
}
extern "C" void Reset_Handler();
#endif

int main()
{
    HAL_Init();
    SystemClock_Config();
    Wdt::Get()->Start(10000000);
    Drivers::AntBms ant = Drivers::AntBms(bmsUart::Get());
    dac->ChannelInit(1);
#ifdef DEBUG
    cli.Open(115200);
    cli.AddCmd(cmd_t{"bootloader", NULL, EnterBootloader, NULL});
    cli.AddCmd(cmd_t{"print_soc", NULL, stateSOC, NULL});
    cli.AddCmd(cmd_t{"dac_test", NULL, stateDacTestMode, NULL});
    cli.AddCmd(cmd_t{"soc_test", NULL, stateSOCTestMode, NULL});
    cli.AddCmd(cmd_t{"dac_val", NULL, (void (*)())dacSetVal, UINT16});
    cli.AddCmd(cmd_t{"soc_val", NULL, (void (*)())socSetVal, UINT16});
    cli.AddCmd(cmd_t{"clear", NULL, clearHeader, NULL});
    cli.AddCmd(cmd_t{"reset", NULL, Reset_Handler, NULL});
#endif
    while (1)
    {
        if (!liveDataReadLastTime || (HAL_GetTick() - liveDataReadLastTime) > 2000)
        {
            livedata = ant.ReadLiveData(valid);
#ifdef DEBUG
            if (testMode != None)
                livedata.Struct.SOC = test_SOC;
            if (valid || testMode == SOCTestMode)
#else
            if (valid)
#endif
            {
                if (livedata.Struct.SOC >= 5 && livedata.Struct.SOC < 25)
                    dacValCh1 = dacTable[1];
                else if (livedata.Struct.SOC >= 25 && livedata.Struct.SOC < 50)
                    dacValCh1 = dacTable[2];
                else if (livedata.Struct.SOC >= 50 && livedata.Struct.SOC < 75)
                    dacValCh1 = dacTable[3];
                else if (livedata.Struct.SOC >= 75 && livedata.Struct.SOC <= 100)
                    dacValCh1 = dacTable[4];
                else
                    dacValCh1 = dacTable[0];
#ifdef DEBUG
                if (testMode != DacTestMode)
#endif
                    dac->SetValue(dacValCh1, 1);
            }
            liveDataReadLastTime = HAL_GetTick();
        }
        Wdt::Get()->ResetCounter();
#ifdef DEBUG
        cli.Loop(HAL_GetTick());
#endif
    }
}