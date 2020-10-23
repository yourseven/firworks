/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-10     LEGION       the first version
 */
#include "st7735.h"
#include <board.h>
//#include "TFT_ST7735/image.h"
#include <stdlib.h>
#include "drv_usart.h"
/*****************************************
WHITE            0xFFFF   白
BLACK            0x0000   黑
BLUE             0x001F   蓝
BRED             0XF81F   蓝红
GRED             0XFFE0   绿红
GBLUE            0X07FF   蓝绿
RED              0xF800   红
MAGENTA          0xF81F   品红
GREEN            0x07E0   绿
CYAN             0x7FFF   青
YELLOW           0xFFE0   黄
BROWN            0XBC40   棕
BRRED            0XFC07   棕红
GRAY             0X8430   灰
DARKBLUE         0X01CF   深蓝
LIGHTBLUE        0X7D7C   浅蓝
GRAYBLUE         0X5458   灰蓝
LIGHTGREEN       0X841F   浅绿
LGRAY            0XC618   浅灰     窗体背景色
LGRAYBLUE        0XA651   浅灰蓝 中间层颜色
LBBLUE           0X2B12   浅棕蓝 选择条目的反色
**********************************************/
struct spi_lcd st7355;
//定义延时函数
#define LCD_Delay_ms(ms)         rt_thread_mdelay(ms)


//开平方函数 形参可为零
static float my_sqrt(float x)
{
    float xhalf = 0.5f*x;
       int i = *(int*)&x;
       i = 0x5f375a86- (i>>1);
       x = *(float*)&i;
       x = x*(1.5f-xhalf*x*x);
       x = x*(1.5f-xhalf*x*x);
       x = x*(1.5f-xhalf*x*x);
       return 1/x;
}
//获取颜色
uint16_t Get_LCD_Color(Lcd_RGB *color)
{
    uint16_t color_t;
    color_t = (color->red << 11) | (color->green << 5) | color->blue;
    return color_t;
}
//发送1字节  state 1 为发送命令    0 为发送数据
void LCD_WR_Byte(uint8_t data, LCD_STATE state)
{
    if(!state)HAL_GPIO_WritePin(st7355.Gpio_dc, st7355.Pin_Dc, GPIO_PIN_SET);
    rt_spi_send(&st7355.sample, &data, 1);
    if(!state)HAL_GPIO_WritePin(st7355.Gpio_dc, st7355.Pin_Dc, GPIO_PIN_RESET);
}
//发送2字节  一个像素点
void LCD_WR_Data(uint16_t data)
{
    LCD_WR_Byte(data >> 8, OLED_DATA);
    LCD_WR_Byte(data,      OLED_DATA);
}
//设置图形坐标 XY  左上角坐标  以及宽高
void LCD_Address_Set(uint16_t x, uint16_t y, uint16_t width, uint16_t high)
{
    LCD_WR_Byte(0x2a, OLED_CMD);
    LCD_WR_Data(x + 1);
    LCD_WR_Data(x + width);
    LCD_WR_Byte(0x2b, OLED_CMD);
    LCD_WR_Data(y);
    LCD_WR_Data(y + high);
    LCD_WR_Byte(0x2c, OLED_CMD);
}

/*********************************************************************************/
//方形图片初始化，设定宽高
static struct my_image *image_Square_init(uint16_t width, uint16_t high)
{
    struct my_image *Image = (struct my_image *)rt_malloc(sizeof(struct my_image));
    //rt_kprintf("Saddr = %x\r\n", &Image);
    if(Image == RT_NULL){
        rt_kprintf("申请Image空间失败");
        return RT_NULL;
    }
    Image->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    Image->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    for(uint32_t i = 0; i < high; i++)
    {
        Image->data[i] = (uint16_t *)rt_malloc(sizeof(uint16_t));
        //rt_kprintf("saddr = %x\r\n", &Image->data[i]);
        if(Image->data[i] == RT_NULL)rt_kprintf("申请Image空间失败");
    }
    Image->width = width;
    Image->high  = high;
    return Image;
}

static struct my_circle *image_Circle_init(uint16_t r, uint16_t max_r)
{
    struct my_circle *Image = (struct my_circle *)rt_malloc(sizeof(struct my_circle));
    if(Image == RT_NULL){
        //rt_kprintf("Caddr = %x\r\n", &Image);
        rt_kprintf("申请Image空间失败");
        return RT_NULL;
    }
    Image->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    Image->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    for(uint32_t i = 0; i < 2 * max_r; i++)
    {
        Image->data[i] = (uint16_t *)rt_malloc(sizeof(uint16_t));
        //rt_kprintf("caddr = %x\r\n", &Image->data[i]);
        if(Image->data[i] == RT_NULL)rt_kprintf("申请Image空间失败");
    }
    Image->max_r = max_r;
    Image->r = r;
    return Image;
}

void *image_init(uint16_t width, uint16_t high, shape_t shpet)
{
    void * Image = RT_NULL;
    switch (shpet) {
        case Square:
            Image = image_Square_init(width, high);
            break;
        case Circle:
            Image = image_Circle_init(width, high);
            break;
        default:
            rt_kprintf("图形初始化失败，未发现此类图形");
            break;
    }
    return Image;
}
/*********************************************************************************/

//从数据整理成一张方形图片
void get_image(struct my_image *Image, uint16_t *data)
{
    for(int i = 0; i < Image->high; i++)
    {
        Image->data[i] = data;
        data += Image->width;
    }
}

/*********************************************************************************/
//对image图片进行裁剪  XY为左上角坐标 设置宽高
static void creat_Square(struct my_image * Image, struct my_image * image, uint16_t x, uint16_t y)
{
    for(uint16_t i = 0; i < Image->high; i++)
    {
        Image->data[i] = image->data[y++] + x;
    }
}

//从image图片中裁剪一张圆形图片,获取一张以image中的XY为圆心，半径为r
static void creat_Circle(struct my_circle * Image, struct my_image * image, uint16_t x, uint16_t y)
{
    int count;
    for(int i = 0; i < Image->max_r; i++)
    {
        count                  = (int)(my_sqrt(Image->max_r * Image->max_r - i * i) + 0.4);
        Image->data[2 * i]     = image->data[y - i] + x - count;
        Image->data[2 * i + 1] = image->data[y + i] + x - count;
    }
}
void creat_Image(void * Image, struct my_image * image, uint16_t x, uint16_t y, shape_t shpet)
{
    switch (shpet) {
        case Square:
            creat_Square(Image, image, x, y);
            break;
        case Circle:
            creat_Circle(Image, image, x, y);
            break;
        default:
            rt_kprintf("图形设置失败，未发现此类图形");
            break;
    }
}
/*********************************************************************************/
//在LCD屏上显示图片image
void LCD_Image(struct my_image *image)
{
    rt_enter_critical();
    LCD_Address_Set(*image->x, *image->y, image->width, image->high);
    for(uint16_t i = 0; i < image->high; i++)
    {
        for(uint16_t j = 0; j < image->width; j++)
        {
            LCD_WR_Data(image->data[i][j]);
        }
    }
    LCD_Delay_ms(1);
    rt_exit_critical();
}

//在LCD屏上显示image圆形图片
void LCD_Circle(struct my_circle *image)
{
    uint32_t count, count2;
    if(image->r > image->max_r)return;
    rt_enter_critical();
    for(int i = 0; i < image->r; i++)
    {
        count = (int)(my_sqrt(image->r * image->r - i * i) + 0.4);
        count2 = (int)(my_sqrt(image->max_r * image->max_r - i * i) + 0.4);
        LCD_Address_Set(*image->x - count, *image->y - i, 2 * count, 1);
        for(int j = 0; j < 2 * count; j++)
            LCD_WR_Data(image->data[2 * i][count2 - count + j]);
        LCD_Address_Set(*image->x - count, *image->y + i, 2 * count, 1);
        for(int j = 0; j < 2 * count; j++)
            LCD_WR_Data(image->data[2 * i + 1][count2 - count + j]);
       /* do{
            if(image->y - i < 0)break;         //上越界
            if(image->r > image->x)            //左越界
            {
                LCD_Address_Set(0, image->y - i, image->x + count, 1);
                for(int j = 0; j < image->x + count; j++)
                    LCD_WR_Data(image->data[2 * i][count2 - image->x + j]);
                break;
            }
            else if(image->r + image->x > 160)  //右越界                                    ////
            {
                LCD_Address_Set(image->x - count, image->y - i, 160 - image->x + count, 1); ////
                for(int j = 0; j < 160 - image->x + count; j++)                            ////
                    LCD_WR_Data(image->data[2 * i][count2 - count + j]);
                break;
            }
            LCD_Address_Set(image->x - count, image->y - i, 2 * count, 1);
            for(int j = 0; j < 2 * count; j++)
                LCD_WR_Data(image->data[2 * i][count2 - count + j]);

        }while(0);
        if(image->y + i >= 128)continue; //下越界                                                 ////
        if(image->r > image->x)          //左越界
        {
            LCD_Address_Set(0, image->y + i, image->x + count, 1);
            for(int j = 0; j < image->x + count; j++)
                LCD_WR_Data(image->data[2 * i + 1][count2 - image->x + j]);
            continue;
        }
        else if(image->r + image->x > 160)  //右越界                                          ////
        {
            LCD_Address_Set(image->x - count, image->y + i, 160 - image->x + count, 1);      ////
            for(int j = 0; j < 160 - image->x + count; j++)                                  ////
                LCD_WR_Data(image->data[2 * i + 1][count2 - count + j]);
            continue;
                    }
        LCD_Address_Set(image->x - count, image->y + i, 2 * count, 1);
        for(int j = 0; j < 2 * count; j++)
            LCD_WR_Data(image->data[2 * i + 1][count2 - count + j]);*/
    }
    LCD_Delay_ms(1);
    rt_exit_critical();
}
/*********************************************************************************/
//注销图片image
void destroy_image(struct my_image *image)
{
    for(int i = 0; i < image->high; i++)
        rt_free(image->data[i]);
    rt_free(image->x);
    rt_free(image->y);
    rt_free(image);
}

//注销image圆形图片
void destroy_circle(struct my_circle * image)
{
    for(int i = 0; i < 2 * image->r; i++)
    rt_free(image->data[i]);
    rt_free(image);
}
/*********************************************************************************/
void LCD_Fill_Circle(struct my_circle *image, uint16_t color)
{
    int count;
    if(image->r > image->max_r)return;
    rt_enter_critical();
    for(int i = 0; i < image->r; i++)
    {
        count = (int)(my_sqrt(image->r * image->r - i * i) + 0.4);
        LCD_Address_Set(*image->x - count, *image->y - i, 2 * count, 1);
        for(int j = 0; j < 2 * count; j++)LCD_WR_Data(color);
        LCD_Address_Set(*image->x - count, *image->y + i, 2 * count, 1);
        for(int j = 0; j < 2 * count; j++)LCD_WR_Data(color);
    }
    LCD_Delay_ms(1);
    rt_exit_critical();
}

//以color颜色填充屏幕
void LCD_Fill(uint16_t x, uint16_t y, uint16_t width, uint16_t high, uint16_t color)
{
    rt_enter_critical();
    LCD_Address_Set(x, y, width, high);
    for(int i = 0; i < high; i++)
    {
        for(int j = 0; j < width; j++)
        {
            LCD_WR_Data(color);
        }
    }
    LCD_Delay_ms(1);
    rt_exit_critical();
}
//选择LCD屏模式    0左上 1右上 2 左下 3右下为原点
void lcd_mode(int mode)
{
    uint8_t mode_t;
    if(mode == 0)mode_t = 0x00;
    else if(mode == 1)mode_t = 0xC0;
    else if(mode == 2)mode_t = 0x70;
    else if(mode == 3)mode_t = 0xA0;
    LCD_WR_Byte(0x36, OLED_CMD);
    LCD_WR_Byte(mode_t, OLED_DATA);
}
//初始化ST7735
static void ST7735_Init(void)
{
    LCD_Delay_ms(100);
    LCD_WR_Byte(0x11, OLED_CMD);
    LCD_Delay_ms(120);

    LCD_WR_Byte(0xB1, OLED_CMD);
    LCD_WR_Byte(0x05, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);

    LCD_WR_Byte(0xB2, OLED_CMD);
    LCD_WR_Byte(0x05, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);

    LCD_WR_Byte(0xB3, OLED_CMD);
    LCD_WR_Byte(0x05, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);
    LCD_WR_Byte(0x05, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);
    LCD_WR_Byte(0x3C, OLED_DATA);

    LCD_WR_Byte(0xB4, OLED_CMD);
    LCD_WR_Byte(0x03, OLED_DATA);

    LCD_WR_Byte(0xC0, OLED_CMD);
    LCD_WR_Byte(0x28, OLED_DATA);
    LCD_WR_Byte(0x08, OLED_DATA);
    LCD_WR_Byte(0x04, OLED_DATA);

    LCD_WR_Byte(0xC1, OLED_CMD);
    LCD_WR_Byte(0xC0, OLED_DATA);

    LCD_WR_Byte(0xC2, OLED_CMD);
    LCD_WR_Byte(0x0D, OLED_DATA);
    LCD_WR_Byte(0x00, OLED_DATA);

    LCD_WR_Byte(0xC3, OLED_CMD);
    LCD_WR_Byte(0x8D, OLED_DATA);
    LCD_WR_Byte(0x2A, OLED_DATA);

    LCD_WR_Byte(0xC4, OLED_CMD);
    LCD_WR_Byte(0x8D, OLED_DATA);
    LCD_WR_Byte(0xEE, OLED_DATA);

    LCD_WR_Byte(0xC5, OLED_CMD);
    LCD_WR_Byte(0x1A, OLED_DATA);

    LCD_WR_Byte(0x36, OLED_CMD);
    LCD_WR_Byte(0x00, OLED_DATA);

    LCD_WR_Byte(0xE0, OLED_CMD);
    LCD_WR_Byte(0x04, OLED_DATA);
    LCD_WR_Byte(0x22, OLED_DATA);
    LCD_WR_Byte(0x07, OLED_DATA);
    LCD_WR_Byte(0x0A, OLED_DATA);
    LCD_WR_Byte(0x2E, OLED_DATA);
    LCD_WR_Byte(0x30, OLED_DATA);
    LCD_WR_Byte(0x25, OLED_DATA);
    LCD_WR_Byte(0x2A, OLED_DATA);
    LCD_WR_Byte(0x28, OLED_DATA);
    LCD_WR_Byte(0x26, OLED_DATA);
    LCD_WR_Byte(0x2E, OLED_DATA);
    LCD_WR_Byte(0x3A, OLED_DATA);
    LCD_WR_Byte(0x01, OLED_DATA);
    LCD_WR_Byte(0x03, OLED_DATA);
    LCD_WR_Byte(0x13, OLED_DATA);

    LCD_WR_Byte(0xE1, OLED_CMD);
    LCD_WR_Byte(0x04, OLED_DATA);
    LCD_WR_Byte(0x16, OLED_DATA);
    LCD_WR_Byte(0x06, OLED_DATA);
    LCD_WR_Byte(0x0D, OLED_DATA);
    LCD_WR_Byte(0x2D, OLED_DATA);
    LCD_WR_Byte(0x26, OLED_DATA);
    LCD_WR_Byte(0x23, OLED_DATA);
    LCD_WR_Byte(0x27, OLED_DATA);
    LCD_WR_Byte(0x27, OLED_DATA);
    LCD_WR_Byte(0x25, OLED_DATA);
    LCD_WR_Byte(0x2D, OLED_DATA);
    LCD_WR_Byte(0x3B, OLED_DATA);
    LCD_WR_Byte(0x00, OLED_DATA);
    LCD_WR_Byte(0x01, OLED_DATA);
    LCD_WR_Byte(0x04, OLED_DATA);
    LCD_WR_Byte(0x13, OLED_DATA);

    LCD_WR_Byte(0x3A, OLED_CMD);
    LCD_WR_Byte(0x05, OLED_DATA);
    LCD_WR_Byte(0x35, OLED_CMD);
    LCD_WR_Byte(0x29, OLED_CMD);
    lcd_mode(3);
}
//初始化ST7735
static int SPI_ST7735_Init(void)
{
    __MY_SPI_INIT((&st7355.sample), SPI1, A, 5, A, 6, A, 7, A, 11);
    SPI_MODE((&st7355.sample), HIGH, 2, 2);
    __ST7735_GPIO_INIT((&st7355), A, 4, A, 8, A, 12);
    MX_SPIx_Init(&st7355.sample);
    ST7735_Init();
    return 0;
}
/*********************************************************************************/
INIT_DEVICE_EXPORT(SPI_ST7735_Init);
#if 0
static void st_thread_entry(void *parameter)
{
    struct my_image  *image1 = get_image(40, 10, (uint16_t *)gImage_shoot);
    struct my_image  *f0     = get_image(36, 36, (uint16_t *)gImage_flower3);
    struct my_circle *c1     = creat_circle(f0, 18, 18, 18);
    if(c1 == RT_NULL)return;
    struct my_image  *image2 = creat_image(f0, 0, 0, 36, 36);
    LCD_Fill(0, 0, 160, 128, 0x00);
    while(1)
    {

    }
}
//初始化ST7735
static int st7735_init(void)
{

    //SPI_ST7735_Init();
    rt_thread_t st7735_thread = rt_thread_create("st7735", st_thread_entry, RT_NULL, 1024, 24, 20);
    if (st7735_thread != RT_NULL)
    {
        rt_thread_startup(st7735_thread);
    }
    return 0;
}

INIT_APP_EXPORT(st7735_init);
#endif
