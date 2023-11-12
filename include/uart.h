#pragma once
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "cfifo.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TX_BUFF_SIZE 256
#define RX_BUFF_SIZE 256

    bool Uart1Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t));

    bool Uart2Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t));

    bool Uart3Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t));

    void UartWrite(uint32_t uart, uint8_t *buffer, uint32_t count);

    uint32_t UartGetReceived(uint32_t uart, uint8_t *buffer, uint32_t size);

#ifdef __cplusplus
}
#endif