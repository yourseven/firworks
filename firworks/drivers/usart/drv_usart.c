/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-09     xiangxistu   first version
 * 2020-05-18     chenyaxing   modify usart3 remap check
 */

#include "drv_common.h"
#include "drv_usart.h"



#define DBG_TAG              "drv.usart"

#ifdef DRV_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif

#include <rtdbg.h>

#ifdef BSP_USING_UART1
struct rt_hw_uart my_uart1 = {"uart1", 1, 115200};

int rt_hw_usart_init(void)
{
    __USART_INIT((&my_uart1), USART1, DMA1, 5);
    __USART_GPIO_INIT(A, 9, A, 10);
    HAL_UART_Receive_DMA(&my_uart1.handle, my_uart1.rx_buffer, 512);
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

void UART1_SEND_DATA(uint8_t *Data, uint16_t Len)
{
    HAL_UART_Transmit(&my_uart1.handle, Data, Len, 0xff);
}

void USART1_IRQHandler(void)
{

    if(__HAL_UART_GET_FLAG(&my_uart1.handle, UART_FLAG_IDLE) != RESET){
        rt_interrupt_enter();
        __HAL_UART_CLEAR_IDLEFLAG(&my_uart1.handle);
        HAL_UART_DMAStop(&my_uart1.handle);
        my_uart1.data_len  = 512 - __HAL_DMA_GET_COUNTER(&my_uart1.hdma_usart_rx);
        rt_sem_release(&my_uart1.sem);
        HAL_UART_Receive_DMA(&my_uart1.handle, my_uart1.rx_buffer, 512);
        rt_interrupt_leave();
    }

}


void Usart_Send_Reg(UsartCallBack ch)
{
    my_uart1.UsartCallBack = ch;
}

static void serial_thread_entry(void *parameter)
{

    while(1)
    {
        rt_sem_take(&my_uart1.sem, RT_WAITING_FOREVER);
        my_uart1.UsartCallBack(my_uart1.rx_buffer, my_uart1.data_len);
    }
}


static int uart_init(void)
{
    rt_sem_init(&my_uart1.sem, "uart1_rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_thread_t usart_thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 512, 24, 20);
    if (usart_thread != RT_NULL)
    {
        rt_thread_startup(usart_thread);
    }
    return 0;
}

INIT_DEVICE_EXPORT(uart_init);
#endif

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
     size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        UART1_SEND_DATA((uint8_t *)(str + i), 1);
    }
}
