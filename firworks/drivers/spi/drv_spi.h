/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-24     LEGION       the first version
 */
#ifndef DRIVERS_DRV_SPI_H_
#define DRIVERS_DRV_SPI_H_

#include "stm32f1xx_hal.h"

enum spi_gpio_tpye{
    SPI_SCL = 0,
    SPI_MISO,
    SPI_MOSI,
    SPI_CSS
};


#define __SPI_GPIO_INIT(gpio, gpio_index, spi_gpio_tpye)             \
do{                                                                  \
    GPIO_TypeDef *GPIOx    = GPIO##gpio;                             \
    uint32_t gpio_pin_name = GPIO_PIN_##gpio_index;                  \
    __HAL_RCC_GPIO##gpio##_CLK_ENABLE();                             \
    GPIO_InitStruct.Pin = gpio_pin_name;                             \
    if((spi_gpio_tpye == SPI_SCL) || (spi_gpio_tpye == SPI_MOSI)){   \
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;                     \
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                \
    }                                                                \
    else if(spi_gpio_tpye == SPI_MISO){                              \
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                      \
        GPIO_InitStruct.Pull = GPIO_NOPULL;                          \
    }                                                                \
    else{                                                            \
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;                 \
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                \
    }                                                                \
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);                          \
}while(0)                                                            \


#define __MY_SPI_INIT(RT_HW_SPI, SPI, SCL_G, SCL_P, MISO_G, MISO_P, MOSI_G, MOSI_P, CS_G, CS_P)\
do{                                                                  \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                          \
    __HAL_RCC_##SPI##_CLK_ENABLE();                                  \
    __SPI_GPIO_INIT(SCL_G, SCL_P, SPI_SCL);                          \
    __SPI_GPIO_INIT(MISO_G, MISO_P, SPI_MISO);                       \
    __SPI_GPIO_INIT(MOSI_G, MOSI_P, SPI_MOSI);                       \
    __SPI_GPIO_INIT(CS_G, CS_P, SPI_CSS);                            \
    RT_HW_SPI->gpio_cs       = GPIO##CS_G;                           \
    RT_HW_SPI->cs_pin_name   = GPIO_PIN_##CS_P;                      \
    RT_HW_SPI->SPIx.Instance = SPI;                                  \
    HAL_GPIO_WritePin(GPIO##CS_G, GPIO_PIN_##CS_P, GPIO_PIN_SET);    \
}while(0)                                                            \


//
//(CLKPola HIGH LOW) (CLKPha 0 1) (BaudR 72/BaudR MHz)
#define SPI_MODE(RT_HW_SPI, CLKPola, CLKPha, BaudR)                        \
{                                                                          \
    RT_HW_SPI->SPIx.Init.CLKPolarity       = SPI_POLARITY_##CLKPola;       \
    RT_HW_SPI->SPIx.Init.CLKPhase          = SPI_PHASE_##CLKPha##EDGE;     \
    RT_HW_SPI->SPIx.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_##BaudR;\
}                                                                          \



struct rt_hw_spi{
    uint32_t              cs_pin_name;
    GPIO_TypeDef          *gpio_cs;
    SPI_HandleTypeDef     SPIx;                               //

};


uint8_t rt_spi_send(struct rt_hw_spi *rt_hw_spi, uint8_t *data, uint16_t len);
uint8_t rt_spi_send_then_recv(struct rt_hw_spi *rt_hw_spi, uint8_t *data1, uint16_t len1, uint8_t *data2, uint16_t len2);
uint8_t rt_spi_send_then_send(struct rt_hw_spi *rt_hw_spi, uint8_t *data1, uint16_t len1, uint8_t *data2, uint16_t len2);
void MX_SPIx_Init(struct rt_hw_spi *my_spi);



#endif /* DRIVERS_DRV_SPI_H_ */
