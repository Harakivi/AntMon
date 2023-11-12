#include "uart.h"

typedef struct
{
    UART_HandleTypeDef *halHandle;
    uint8_t txBuff[TX_BUFF_SIZE];
    uint8_t rxBuff[RX_BUFF_SIZE];
    cfifo txFifo;
    cfifo rxFifo;
    void (*_onByteReceived)(uint8_t);
} uart_t;
uart_t *uarts_data[3] = {0};

bool Uart1Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t))
{
    static UART_HandleTypeDef halHandle = {0};
    static uart_t uart1_data = {0};
    uarts_data[0] = &uart1_data;
    uart1_data.halHandle = &halHandle;
    halHandle.Instance = USART1;
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
        cfifo_init(&uart1_data.txFifo, uart1_data.txBuff, TX_BUFF_SIZE);
        cfifo_init(&uart1_data.rxFifo, uart1_data.rxBuff, RX_BUFF_SIZE);
        uart1_data._onByteReceived = onByteReceived;
    }
    return res;
}

bool Uart2Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t))
{
    static UART_HandleTypeDef halHandle = {0};
    static uart_t uart2_data = {0};
    uarts_data[1] = &uart2_data;
    uart2_data.halHandle = &halHandle;
    halHandle.Instance = USART2;
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
        cfifo_init(&uart2_data.txFifo, uart2_data.txBuff, TX_BUFF_SIZE);
        cfifo_init(&uart2_data.rxFifo, uart2_data.rxBuff, RX_BUFF_SIZE);
        uart2_data._onByteReceived = onByteReceived;
    }
    return res;
}

bool Uart3Open(uint32_t BaudRate, void (*onByteReceived)(uint8_t))
{
    static UART_HandleTypeDef halHandle = {0};
    static uart_t uart3_data = {0};
    uarts_data[2] = &uart3_data;
    uart3_data.halHandle = &halHandle;
    halHandle.Instance = USART3;
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
        cfifo_init(&uart3_data.txFifo, uart3_data.txBuff, TX_BUFF_SIZE);
        cfifo_init(&uart3_data.rxFifo, uart3_data.rxBuff, RX_BUFF_SIZE);
        uart3_data._onByteReceived = onByteReceived;
    }
    return res;
}

void UartWrite(uint32_t uart, uint8_t *buffer, uint32_t count)
{
    uint32_t writted = 0;

    while (writted != count)
    {
        if (cfifo_push(&uarts_data[uart - 1]->txFifo, buffer + writted))
        {
            writted++;
        }
    }

    uarts_data[uart - 1]->halHandle->Instance->CR1 |= USART_CR1_TXEIE;
}

uint32_t UartGetReceived(uint32_t uart, uint8_t *buffer, uint32_t size)
{
    uint32_t readed = 0;
    while (uarts_data[uart - 1]->rxFifo.length && readed < size)
    {
        if (cfifo_pop(&uarts_data[uart - 1]->rxFifo, (uint8_t *)buffer + readed))
        {
            readed++;
        }
    }
    return readed;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART1)
    {

        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        huart->Instance->CR1 |= USART_CR1_RXNEIE;

        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    if (huart->Instance == USART2)
    {

        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        huart->Instance->CR1 |= USART_CR1_RXNEIE;

        HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }

    if (huart->Instance == USART3)
    {

        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        huart->Instance->CR1 |= USART_CR1_RXNEIE;

        HAL_NVIC_SetPriority(USART3_IRQn, 3, 3);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
}

void UART_Handler(uart_t *uart)
{
    if (uart->halHandle->Instance->SR & USART_SR_RXNE)
    {
        uint8_t byte = uart->halHandle->Instance->DR & 0xff;
        if (uart->_onByteReceived != NULL)
        {
            uart->_onByteReceived(byte);
            uart->halHandle->Instance->SR &= ~USART_SR_RXNE;
        }
        else if (cfifo_push(&uart->rxFifo, &byte))
        {
            uart->halHandle->Instance->SR &= ~USART_SR_RXNE;
        }
    }

    if (uart->halHandle->Instance->SR & USART_SR_TXE)
    {
        uart->halHandle->Instance->SR &= ~USART_SR_TXE;
        if (uart->txFifo.length)
        {
            uint8_t byte;
            if (cfifo_pop(&uart->txFifo, &byte))
            {
                uart->halHandle->Instance->DR = byte;
                return;
            }
        }
        uart->halHandle->Instance->CR1 &= ~USART_CR1_TXEIE;
    }
}

void USART1_IRQHandler()
{
    if (uarts_data[0] != NULL)
        UART_Handler(uarts_data[0]);
}

void USART2_IRQHandler()
{
    if (uarts_data[1] != NULL)
        UART_Handler(uarts_data[1]);
}

void USART3_IRQHandler()
{
    if (uarts_data[2] != NULL)
        UART_Handler(uarts_data[2]);
}