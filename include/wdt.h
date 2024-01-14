#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool WdtStart(uint32_t period_us);

    void WdtResetCounter();

#ifdef __cplusplus
}
#endif