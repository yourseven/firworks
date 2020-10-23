/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-10     LEGION       the first version
 */
#ifndef APPLICATIONS_TFT_ST7735_ST7735_H_
#define APPLICATIONS_TFT_ST7735_ST7735_H_

#include "drv_spi.h"

#define __ST7735_GPIO_INIT(spi_lcd, g_res, p_res, g_dc, p_dc, g_bl, p_bl)    \
do{                                                                          \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                  \
    __HAL_RCC_GPIO##g_res##_CLK_ENABLE();                                    \
    GPIO_InitStruct.Pin   = GPIO_PIN_##p_res;                                \
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;                             \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                            \
    HAL_GPIO_Init(GPIO##g_res, &GPIO_InitStruct);                            \
                                                                             \
    __HAL_RCC_GPIO##g_dc##_CLK_ENABLE();                                     \
    GPIO_InitStruct.Pin   = GPIO_PIN_##p_dc;                                 \
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;                             \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                            \
    HAL_GPIO_Init(GPIO##g_dc, &GPIO_InitStruct);                             \
                                                                             \
    __HAL_RCC_GPIO##g_bl##_CLK_ENABLE();                                     \
    GPIO_InitStruct.Pin   = GPIO_PIN_##p_bl;                                 \
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;                             \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                            \
    HAL_GPIO_Init(GPIO##g_bl, &GPIO_InitStruct);                             \
                                                                             \
    spi_lcd->Pin_Res      = GPIO_PIN_##p_res;                                \
    spi_lcd->Pin_Dc       = GPIO_PIN_##p_dc;                                 \
    spi_lcd->Pin_Bl       = GPIO_PIN_##p_bl;                                 \
    spi_lcd->Gpio_res     = GPIO##g_res;                                     \
    spi_lcd->Gpio_dc      = GPIO##g_dc;                                      \
    spi_lcd->Gpio_bl      = GPIO##g_bl;                                      \
                                                                             \
    HAL_GPIO_WritePin(GPIO##g_res, GPIO_PIN_##p_res, GPIO_PIN_SET);          \
    HAL_GPIO_WritePin(GPIO##g_dc,  GPIO_PIN_##p_dc,  GPIO_PIN_RESET);        \
    HAL_GPIO_WritePin(GPIO##g_bl,  GPIO_PIN_##p_bl,  GPIO_PIN_SET);          \
}while(0)                                                                    \


typedef enum
{
  OLED_DATA = 0u,
  OLED_CMD
}LCD_STATE;

typedef struct RGB{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}Lcd_RGB;

struct spi_lcd{
    uint32_t              Pin_Res;
    uint32_t              Pin_Dc;
    uint32_t              Pin_Bl;
    GPIO_TypeDef          *Gpio_res;
    GPIO_TypeDef          *Gpio_dc;
    GPIO_TypeDef          *Gpio_bl;
    struct rt_hw_spi      sample;
};

//方形图片结构体
struct my_image{
    uint16_t *x;
    uint16_t *y;
    uint16_t width;
    uint16_t high;
    uint16_t *data[];
};

//圆形图片结构体
struct my_circle{
    uint16_t *x;
    uint16_t *y;
    uint16_t r;
    uint16_t max_r;
    uint16_t *data[];
};

typedef enum image_shape{
    Square,     //方形图片
    Circle,     //圆形图片
}shape_t;



uint16_t Get_LCD_Color(Lcd_RGB *color);
void LCD_WR_Data(uint16_t data);
void LCD_WR_Byte(uint8_t data, LCD_STATE state);
void LCD_Address_Set(uint16_t x, uint16_t y, uint16_t width, uint16_t high);

void *image_init(uint16_t width, uint16_t high, shape_t shpet);
void creat_Image(void * Image, struct my_image * image, uint16_t x, uint16_t y, shape_t shpet);
void get_image(struct my_image *Image, uint16_t *data);
void LCD_Image(struct my_image *image);
void destroy_image(struct my_image * image);
void LCD_Circle(struct my_circle *image);
void destroy_circle(struct my_circle * image);
void LCD_Fill_Circle(struct my_circle *image, uint16_t color);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
#endif /* APPLICATIONS_TFT_ST7735_ST7735_H_ */
