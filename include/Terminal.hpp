#pragma once
#include <stdint.h>
#include "stm32f1xx_hal.h"

#define TERMINAL_UART USART1
#define HAL_RCC_TERMINAL_UART_CLK_ENABLE __HAL_RCC_USART1_CLK_ENABLE
#define HAL_RCC_TERMINAL_PORT_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE
#define TERMINAL_UART_RX_PORT GPIOA
#define TERMINAL_UART_RX_PIN GPIO_PIN_10
#define TERMINAL_UART_TX_PORT GPIOA
#define TERMINAL_UART_TX_PIN GPIO_PIN_9
#define TERMINAL_UART_IRQn USART1_IRQn
#define TERMINAL_UART_IRQHandler USART1_IRQHandler

#define TX_BUFF_SIZE 256
#define RX_BUFF_SIZE 256

namespace Terminal
{
    class Uart
    {
    private:
        UART_HandleTypeDef halHandle;
        bool _opened;
        Uart() : _opened(false) {}

    public:
        static Uart *Get();

        bool Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t) = nullptr);

        uint32_t Write(uint8_t *buffer, uint32_t count);

        uint32_t print(const char *ch);

        uint32_t GetReceived(uint8_t *buffer, uint32_t size);
    };
}