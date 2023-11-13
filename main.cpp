#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "uart.hpp"
#include "AntBms.hpp"
#include "dac.hpp"

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

typedef InternalPeriph::Uart<1> Cli;
typedef InternalPeriph::Dac Dac;
Cli *cli = Cli::Get();
Dac *dac = Dac::Get();
typedef InternalPeriph::Uart<3> bmsUart;

int main()
{
    HAL_Init();
    SystemClock_Config();
    cli->Open(115200);
    Drivers::AntBms ant = Drivers::AntBms(bmsUart::Get());
    bool valid = true;
    Drivers::AntLiveData livedata = {0};
    uint16_t dacValCh1 = 0;
    dac->ChannelInit(1);
    while (1)
    {
        livedata = ant.ReadLiveData(valid);
        if (valid)
        {
            dacValCh1 = livedata.Struct.SOC * 12 + 2895;
            dac->SetValue(dacValCh1, 1);
            cli->print("\033c");
            if (livedata.Struct.SOC >= 75)
                cli->print("\033[92m");
            else if (livedata.Struct.SOC < 75 && livedata.Struct.SOC >= 35)
                cli->print("\033[93m");
            else if (livedata.Struct.SOC < 35)
                cli->print("\033[91m");
            cli->print("========================\r\n");
            cli->print("AntBms SOC = %d\r\n", livedata.Struct.SOC);
            cli->print("Total Voltage = %d\r\n", (livedata.Struct.TotalVoltage[0] << 8) + livedata.Struct.TotalVoltage[1]);
            cli->print("DAC ch1 Value = %d\r\n", dacValCh1);
            cli->print("========================\r\n");
            HAL_Delay(2000);
        }
    }
}