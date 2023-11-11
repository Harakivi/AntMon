#include "Terminal.hpp"
#include "cfifo.h"

using namespace Terminal;

namespace Terminal
{
    cfifo txFifo = {0};
    cfifo rxFifo = {0};
    void (*_onByteReceived)(uint8_t);
}

Uart *Uart::Get()
{
    static Uart instance = Uart();
    return &instance;
}

bool Uart::Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t))
{
    halHandle = {0};
    halHandle.Instance = TERMINAL_UART;
    halHandle.Init.BaudRate = BaudRate;
    halHandle.Init.WordLength = UART_WORDLENGTH_8B;
    halHandle.Init.StopBits = UART_STOPBITS_1;
    halHandle.Init.Parity = UART_PARITY_NONE;
    halHandle.Init.Mode = UART_MODE_TX_RX;
    halHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    halHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    bool res = HAL_UART_Init(&halHandle) == HAL_OK;
    if (res)
    {
        static uint8_t txBuff[TX_BUFF_SIZE];
        static uint8_t rxBuff[RX_BUFF_SIZE];
        _opened = true;
        cfifo_init(&txFifo, txBuff, TX_BUFF_SIZE);
        cfifo_init(&rxFifo, rxBuff, RX_BUFF_SIZE);
        _onByteReceived = onByteReceived;
    }
    return res;
}

uint32_t Uart::Write(uint8_t *buffer, uint32_t count)
{

    uint32_t writted = 0;
    if (_opened)
    {
        while (writted != count)
        {
            cfifo_push(&txFifo, buffer + writted++);
        }

        TERMINAL_UART->CR1 |= USART_CR1_TXEIE;
    }
    return writted;
}

uint32_t Uart::print(const char *ch)
{
    uint32_t writted = 0;
    if (_opened)
    {
        while (*(ch + writted))
        {
            cfifo_push(&txFifo, (uint8_t *)ch + writted++);
        }

        TERMINAL_UART->CR1 |= USART_CR1_TXEIE;
    }
    return writted;
}

uint32_t Uart::GetReceived(uint8_t *buffer, uint32_t size)
{
    uint32_t writted = 0;
    if (_opened)
    {
        while (rxFifo.length && writted < size)
        {
            cfifo_pop(&rxFifo, (uint8_t *)buffer + writted++);
        }
    }
    return writted;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == TERMINAL_UART)
    {

        HAL_RCC_TERMINAL_UART_CLK_ENABLE();

        HAL_RCC_TERMINAL_PORT_CLK_ENABLE();

        GPIO_InitStruct.Pin = TERMINAL_UART_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(TERMINAL_UART_TX_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = TERMINAL_UART_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(TERMINAL_UART_RX_PORT, &GPIO_InitStruct);

        huart->Instance->CR1 |= USART_CR1_RXNEIE;

        HAL_NVIC_SetPriority(TERMINAL_UART_IRQn, 3, 3);
        HAL_NVIC_EnableIRQ(TERMINAL_UART_IRQn);
    }
}

extern "C" void TERMINAL_UART_IRQHandler()
{
    if (TERMINAL_UART->SR & USART_SR_RXNE)
    {
        uint8_t byte = TERMINAL_UART->DR & 0xff;
        if (_onByteReceived != nullptr)
        {
            _onByteReceived(byte);
            TERMINAL_UART->SR &= ~USART_SR_RXNE;
        }
        else if (cfifo_push(&rxFifo, &byte))
        {
            TERMINAL_UART->SR &= ~USART_SR_RXNE;
        }
    }

    if (TERMINAL_UART->SR & USART_SR_TXE)
    {
        if (txFifo.length)
        {
            uint8_t byte;
            if (cfifo_pop(&txFifo, &byte))
            {
                TERMINAL_UART->DR = byte;
                return;
            }
        }
        TERMINAL_UART->CR1 &= ~USART_CR1_TXEIE;
    }
}