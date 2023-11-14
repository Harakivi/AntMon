#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "uart.hpp"
#include "Cli.hpp"
#include "AntBms.hpp"
#include "dac.hpp"
#include "cfifo.h"

enum CliStates
{
    Clear = 0,
    SOC,
    DacTestMode,
};

extern "C" void SystemClock_Config();

typedef InternalPeriph::Uart<1> cliUart;
typedef InternalPeriph::Uart<3> bmsUart;
typedef InternalPeriph::Dac Dac;
Dac *dac = Dac::Get();

typedef Drivers::Cli Cli;
#ifdef DEBUG
Cli cli = Cli(cliUart::Get());
CliStates cliState;
#endif

bool valid = true;
Drivers::AntLiveData livedata = {0};
uint16_t dacValCh1 = 0;
uint32_t liveDataReadLastTime = 0;

#ifdef DEBUG
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
    cli.printHeader("========================\r\n");
}

void stateSOC()
{
    cliState = SOC;
    cli.setHeaderUpdater(printSOC);
}

void dacSetVal(uint16_t value)
{
    dac->SetValue(value, 1);
}

void printDAC()
{
    cli.clear();
    cli.clearHeader();
    cli.printHeader("========================\r\n");
    cli.printHeader("DAC ch1 Value = %d\r\n", dac->GetValue(1));
    cli.printHeader("========================\r\n");
}

void stateDacTestMode()
{
    cliState = DacTestMode;
    cli.setHeaderUpdater(printDAC);
}

void clearHeader()
{
    cliState = Clear;
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
    Drivers::AntBms ant = Drivers::AntBms(bmsUart::Get());
    dac->ChannelInit(1);
#ifdef DEBUG
    cli.Open(115200);
    cli.AddCmd(cmd_t{"print_soc", NULL, (void *)stateSOC, NULL});
    cli.AddCmd(cmd_t{"dac_test", NULL, (void *)stateDacTestMode, NULL});
    cli.AddCmd(cmd_t{"dac_val", NULL, (void *)dacSetVal, UINT16});
    cli.AddCmd(cmd_t{"clear", NULL, (void *)clearHeader, NULL});
    cli.AddCmd(cmd_t{"reset", NULL, (void *)Reset_Handler, NULL});
#endif
    while (1)
    {
        if (!liveDataReadLastTime || (HAL_GetTick() - liveDataReadLastTime) > 2000)
        {
            livedata = ant.ReadLiveData(valid);
            if (valid)
            {
                dacValCh1 = livedata.Struct.SOC * 12 + 2895;
#ifdef DEBUG
                if (cliState != DacTestMode)
#endif
                    dac->SetValue(dacValCh1, 1);
            }
            liveDataReadLastTime = HAL_GetTick();
        }
#ifdef DEBUG
        cli.Loop(HAL_GetTick());
#endif
    }
}