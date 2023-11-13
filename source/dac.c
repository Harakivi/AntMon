#include "dac.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>

DAC_HandleTypeDef *halHandle = NULL;

bool DacInit()
{
    static DAC_HandleTypeDef hdac = {0};
    halHandle = &hdac;
    __HAL_RCC_DAC_CLK_ENABLE();
    hdac.Instance = DAC;
    return HAL_DAC_Init(&hdac) == HAL_OK;
}

bool DacChannelInit(uint8_t channel)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    uint32_t dac_channel = 0;
    switch (channel)
    {
    case 1:
    {
        dac_channel = DAC_CHANNEL_1;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    break;
    case 2:
    {
        dac_channel = DAC_CHANNEL_2;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    break;

    default:
        break;
    }
    return HAL_DAC_ConfigChannel(halHandle, &sConfig, dac_channel) == HAL_OK && HAL_DAC_Start(halHandle, dac_channel) == HAL_OK;
}

void DacSetValue(uint8_t channel, uint16_t value)
{
    switch (channel)
    {
    case 1:
        HAL_DAC_SetValue(halHandle, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case 2:
        HAL_DAC_SetValue(halHandle, DAC1_CHANNEL_2, DAC_ALIGN_12B_R, value);
        break;

    default:
        break;
    }
}

uint16_t DacGetValue(uint8_t channel)
{
    switch (channel)
    {
    case 1:
        return HAL_DAC_GetValue(halHandle, DAC1_CHANNEL_1);
        break;
    case 2:
        return HAL_DAC_GetValue(halHandle, DAC1_CHANNEL_2);
        break;

    default:
        break;
    }
    return 0;
}