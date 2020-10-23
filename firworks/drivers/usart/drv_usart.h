/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-06     LEGION       the first version
 */
#ifndef DRIVERS_INCLUDE_DRV_USART_H_
#define DRIVERS_INCLUDE_DRV_USART_H_

#define BSP_USING_UART1
//#define BSP_USING_UART2
//#define BSP_USING_UART3

#if defined(BSP_USING_UART1) || defined(BSP_USING_UART2) || defined(BSP_USING_UART3)

#define __USART_GPIO_INIT(GPIO_RX, PIN_RX, GPIO_TX, PIN_TX)                      \
do{                                                                              \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                      \
    __HAL_RCC_GPIO##GPIO_RX##_CLK_ENABLE();                                      \
    __HAL_RCC_GPIO##GPIO_TX##_CLK_ENABLE();                                      \
                                                                                 \
    GPIO_InitStruct.Pin   = GPIO_PIN_##PIN_RX;                                   \
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;                                     \
    GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;                               \
    HAL_GPIO_Init(GPIO##GPIO_RX, &GPIO_InitStruct);                              \
                                                                                 \
    GPIO_InitStruct.Pin   = GPIO_PIN_##PIN_TX;                                   \
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;                                     \
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                          \
    HAL_GPIO_Init(GPIO##GPIO_TX, &GPIO_InitStruct);                              \
}while(0)                                                                        \


#define __USART_INIT(rt_hw_uart, USARTx, DMAx, NUM)                              \
do{                                                                              \
    __HAL_RCC_##DMAx##_CLK_ENABLE();                                             \
    __HAL_RCC_##USARTx##_CLK_ENABLE();                                           \
                                                                                 \
    rt_hw_uart->hdma_usart_rx.Instance                 = DMAx##_Channel##NUM;    \
    rt_hw_uart->hdma_usart_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;   \
    rt_hw_uart->hdma_usart_rx.Init.PeriphInc           = DMA_PINC_DISABLE;       \
    rt_hw_uart->hdma_usart_rx.Init.MemInc              = DMA_MINC_ENABLE;        \
    rt_hw_uart->hdma_usart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;    \
    rt_hw_uart->hdma_usart_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;    \
    rt_hw_uart->hdma_usart_rx.Init.Mode                = DMA_NORMAL;             \
    rt_hw_uart->hdma_usart_rx.Init.Priority            = DMA_PRIORITY_LOW;       \
    if (HAL_DMA_Init(&rt_hw_uart->hdma_usart_rx) != HAL_OK)                      \
    {                                                                            \
        Error_Handler();                                                         \
    }                                                                            \
    __HAL_LINKDMA(&rt_hw_uart->handle, hdmarx, rt_hw_uart->hdma_usart_rx);       \
                                                                                 \
    rt_hw_uart->handle.Instance          = USARTx;                               \
    rt_hw_uart->handle.Init.BaudRate     = rt_hw_uart->BaudRate;                 \
    rt_hw_uart->handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;                  \
    rt_hw_uart->handle.Init.Mode         = UART_MODE_TX_RX;                      \
    rt_hw_uart->handle.Init.OverSampling = UART_OVERSAMPLING_16;                 \
    rt_hw_uart->handle.Init.WordLength   = UART_WORDLENGTH_8B;                   \
    rt_hw_uart->handle.Init.StopBits     = UART_STOPBITS_1;                      \
    rt_hw_uart->handle.Init.Parity       = UART_PARITY_NONE;                     \
                                                                                 \
    if (HAL_UART_Init(&rt_hw_uart->handle) != HAL_OK)                            \
    {                                                                            \
        return -RT_ERROR;                                                        \
    }                                                                            \
                                                                                 \
    HAL_NVIC_SetPriority(USARTx##_IRQn, rt_hw_uart->Priority, 0);                \
    HAL_NVIC_EnableIRQ(USARTx##_IRQn);                                           \
    HAL_NVIC_SetPriority(DMAx##_Channel##NUM##_IRQn, rt_hw_uart->Priority + 1, 0);\
    HAL_NVIC_EnableIRQ(DMAx##_Channel##NUM##_IRQn);                              \
    __HAL_UART_ENABLE_IT(&rt_hw_uart->handle, UART_IT_IDLE);                     \
                                                                                 \
}while(0)                                                                        \


typedef void (*UsartCallBack) (uint8_t *ch, uint16_t  data_len);


struct rt_hw_uart{
    char                        *name;                      //
    uint32_t                    Priority;
    uint32_t                    BaudRate;
    uint16_t                    data_len;
    UART_HandleTypeDef          handle;                     //
    DMA_HandleTypeDef           hdma_usart_rx;
    struct rt_semaphore         sem;
    uint8_t                     rx_buffer[128];

    void (*UsartCallBack) (uint8_t *ch, uint16_t  data_len);
};
#endif

#ifdef BSP_USING_UART1

void UART1_SEND_DATA(uint8_t *Data, uint16_t Len);
void Usart_Send_Reg(UsartCallBack ch);
#endif


#ifdef BSP_USING_UART2

void UART2_SEND_DATA(uint8_t *Data, uint16_t Len, uint32_t Timeout);

void Uasrt2CallBackReg(UsartCallBack Back);

#define UART2_CONFIG                                                \
    {                                                               \
        .name = "uart2",                                            \
        .dma_data_len = my_usart_rev_len,                           \
        .PreemptPriority = 2,                                       \
        .Instance = USART2,                                         \
        .tx_pin_name = GPIO_PIN_2,                                  \
        .rx_pin_name = GPIO_PIN_3,                                  \
        .GPIOx = GPIOA,                                             \
        .DMA_TX_Channel = DMA1_Channel7,                            \
        .DMA_RX_Channel = DMA1_Channel6,                            \
        .IRQn_Type = USART2_IRQn                                    \
    }                                                               \

#endif

#ifdef BSP_USING_UART3

void UART3_SEND_DATA(uint8_t *Data, uint16_t Len, uint32_t Timeout);

void Uasrt3CallBackReg(UsartCallBack Back);

#define UART3_CONFIG                                                \
    {                                                               \
        .name = "uart3",                                            \
        .dma_data_len = my_usart_rev_len,                           \
        .PreemptPriority = 1,                                       \
        .Instance = USART3,                                         \
        .tx_pin_name = GPIO_PIN_10,                                 \
        .rx_pin_name = GPIO_PIN_11,                                 \
        .GPIOx = GPIOB,                                             \
        .DMA_TX_Channel = DMA1_Channel2,                            \
        .DMA_RX_Channel = DMA1_Channel3,                            \
        .IRQn_Type = USART3_IRQn                                    \
    }                                                               \

#endif

#endif /* DRIVERS_INCLUDE_DRV_USART_H_ */
