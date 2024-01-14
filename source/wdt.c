#include "wdt.h"
#include "stm32f1xx_hal.h"
#include "stm32f100xb.h"

static IWDG_HandleTypeDef *hal_iwdg_ptr;

bool WdtStart(uint32_t period_us)
{
    static IWDG_HandleTypeDef hal_iwdg;
    hal_iwdg_ptr = &hal_iwdg;
    if (period_us < (1000000 / (LSI_VALUE / 4)))
        return false;
    if (period_us > ((256000000 / (LSI_VALUE)) * 0x0FFF))
        return false;
    uint32_t prescaler = 4;
    uint8_t prescalerVal = 0;
    uint16_t reloadVal = 0;
    for (int i = 0; i < 7; i++)
    {
        uint32_t calcReloadVal = period_us / ((1000000 * prescaler) / LSI_VALUE);
        if (calcReloadVal <= 0x0FFF)
        {
            reloadVal = calcReloadVal;
            break;
        }
        prescaler <<= 1;
        prescalerVal++;
    }

    hal_iwdg.Instance = IWDG;
    hal_iwdg.Init.Prescaler = prescalerVal;
    hal_iwdg.Init.Reload = reloadVal;
    return HAL_IWDG_Init(hal_iwdg_ptr) == HAL_OK;
}

void WdtResetCounter()
{
    HAL_IWDG_Refresh(hal_iwdg_ptr);
}