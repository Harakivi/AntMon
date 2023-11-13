#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "uart.hpp"
#include "Cli.hpp"
#include "AntBms.hpp"
#include "dac.hpp"
#include "cfifo.h"

UART_HandleTypeDef huart1;

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

typedef InternalPeriph::Uart<1> cliUart;
typedef InternalPeriph::Uart<3> bmsUart;
typedef InternalPeriph::Dac Dac;
typedef Drivers::Cli Cli;

Cli cli = Cli(cliUart::Get());
Dac *dac = Dac::Get();

bool valid = true;
Drivers::AntLiveData livedata = {0};
uint16_t dacValCh1 = 0;
uint32_t liveDataReadLastTime = 0;

void printSOC()
{
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

void clearHeader()
{
    cli.clearHeader();
}

int main()
{
    HAL_Init();
    SystemClock_Config();
    Drivers::AntBms ant = Drivers::AntBms(bmsUart::Get());
    cli.Open(115200);
    cli.AddCmd(cmd_t{"printSOC", NULL, (void*)printSOC, NULL});
    cli.AddCmd(cmd_t{"clearHeader", NULL, (void*)clearHeader, NULL});
    while (1)
    {
        if (!liveDataReadLastTime || (HAL_GetTick() - liveDataReadLastTime) > 2000)
        {
            livedata = ant.ReadLiveData(valid);
            if (valid)
            {
                dacValCh1 = livedata.Struct.SOC * 12 + 2895;
                dac->SetValue(dacValCh1, 1);
            }
            liveDataReadLastTime = HAL_GetTick();
        }
        cli.Loop(HAL_GetTick());
    }
}