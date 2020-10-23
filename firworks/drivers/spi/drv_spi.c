/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-24     LEGION       the first version
 */
#include "drv_spi.h"
#include "drv_common.h"



static void SPI_CSS_DISABLE(struct rt_hw_spi *my_spi)
{
    HAL_GPIO_WritePin(my_spi->gpio_cs, my_spi->cs_pin_name, GPIO_PIN_SET);
}


static void SPI_CSS_ENABLE(struct rt_hw_spi *my_spi)
{
    HAL_GPIO_WritePin(my_spi->gpio_cs, my_spi->cs_pin_name, GPIO_PIN_RESET);
}


//
uint8_t rt_spi_send_then_recv(struct rt_hw_spi *rt_hw_spi, uint8_t *data1, uint16_t len1, uint8_t *data2, uint16_t len2)
{
  uint8_t d_read;
  SPI_CSS_ENABLE(rt_hw_spi);
  if(HAL_SPI_Transmit(&rt_hw_spi->SPIx, data1, len1, 0xFF) ==HAL_OK)
  {
      d_read  = HAL_SPI_Receive(&rt_hw_spi->SPIx, data2, len2, 0xFF);
  }
  else d_read = HAL_ERROR;
  SPI_CSS_DISABLE(rt_hw_spi);
  return d_read;
}

uint8_t rt_spi_send(struct rt_hw_spi *rt_hw_spi, uint8_t *data, uint16_t len)
{
    uint8_t sta;
    SPI_CSS_ENABLE(rt_hw_spi);
    sta = HAL_SPI_Transmit(&rt_hw_spi->SPIx, data, len, 0xff);
    SPI_CSS_DISABLE(rt_hw_spi);
    return sta;
}



//
uint8_t rt_spi_send_then_send(struct rt_hw_spi *rt_hw_spi, uint8_t *data1, uint16_t len1, uint8_t *data2, uint16_t len2)
{
    uint8_t sta;
    SPI_CSS_ENABLE(rt_hw_spi);
    HAL_SPI_Transmit(&rt_hw_spi->SPIx, data1, len1, 0xff);
    sta = HAL_SPI_Transmit(&rt_hw_spi->SPIx, data2, len2, 0xff);
    SPI_CSS_DISABLE(rt_hw_spi);
    return sta;
}




void MX_SPIx_Init(struct rt_hw_spi *my_spi)
{
    my_spi->SPIx.Init.Mode            = SPI_MODE_MASTER;
    my_spi->SPIx.Init.Direction       = SPI_DIRECTION_2LINES;
    my_spi->SPIx.Init.DataSize        = SPI_DATASIZE_8BIT;
    my_spi->SPIx.Init.NSS             = SPI_NSS_SOFT;
    my_spi->SPIx.Init.FirstBit        = SPI_FIRSTBIT_MSB;
    my_spi->SPIx.Init.TIMode          = SPI_TIMODE_DISABLE;
    my_spi->SPIx.Init.CRCCalculation  = SPI_CRCCALCULATION_DISABLE;
    my_spi->SPIx.Init.CRCPolynomial   = 10;
     if (HAL_SPI_Init(&my_spi->SPIx) != HAL_OK)
    {
        Error_Handler();
    }
}



