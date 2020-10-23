/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-19     LEGION       the first version
 */

#include "st7735.h"
#include "firework.h"
#include "TFT_ST7735/image.h"
#include <stdlib.h>

#if 1
struct fireWorks{
    uint16_t  *x;
    uint16_t  *y;
    uint16_t  y1;
    struct my_image *firework;
};
uint16_t color_t = 0x00;
static void firework_init(struct fireWorks *firework, struct my_image *image1, struct my_circle *flower, struct my_image *image2, int i)
{
    *firework->x = rand() % 145 + 5;
    *firework->y = rand() % 20  + 90;
    firework->y1 = rand() % 50;
    creat_Image(firework->firework, image1, 4 * i, 0, Square);
    creat_Image(flower, image2, 18, 18 + 36 * (i % 4), Circle);
    flower->r = 1;

}


static void firework_thread_entry(void *f0)
{
    struct my_image  *image1   = image_init(40, 7, Square);
    get_image(image1, (uint16_t *)gImage_shoot);
    struct my_circle *s1 = image_init(1, 18, Circle);       //烟花
    struct fireWorks *firework = (struct fireWorks *)rt_malloc(sizeof(struct fireWorks)); //烟花弹
    firework->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->firework = image_init(3, 7, Square);
    firework->firework->x = firework->x;
    firework->firework->y = firework->y;
    s1->x = firework->x;
    s1->y = firework->y;
    uint16_t r1;
    int i;
    while(1)
    {
        i = rand() % 10;
        firework_init(firework, image1, s1, f0, i);
        r1 = rand() % 8 + 10;
        do{
            LCD_Image(firework->firework);
            rt_thread_mdelay(4 * i + 5);
            LCD_Fill(*firework->x, *firework->y, firework->firework->width, firework->firework->high, color_t);
            if(*firework->y == firework->y1)break;
        }while((*firework->y)--);
        do{
            LCD_Circle(s1);
            rt_thread_mdelay(5 * i + 20);
            s1->r++;
        }while(s1->r < r1);
        LCD_Fill_Circle(s1, color_t);
        rt_thread_mdelay(500 * i);
    }
}

static void firework_thread_entry1(void *f0)
{
    struct my_image  *image1   = image_init(40, 7, Square);
    get_image(image1, (uint16_t *)gImage_shoot);
    struct my_circle *s1 = image_init(1, 18, Circle);       //烟花
    struct fireWorks *firework = (struct fireWorks *)rt_malloc(sizeof(struct fireWorks)); //烟花弹
    firework->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->firework = image_init(3, 7, Square);
    firework->firework->x = firework->x;
    firework->firework->y = firework->y;
    s1->x = firework->x;
    s1->y = firework->y;
    uint16_t r1;
    int i;
    while(1)
    {
        i = rand() % 10;
        firework_init(firework, image1, s1, f0, i);
        r1 = rand() % 8 + 10;
        do{
            LCD_Image(firework->firework);
            rt_thread_mdelay(8 * i + 5);
            LCD_Fill(*firework->x, *firework->y, firework->firework->width, firework->firework->high, color_t);
            if(*firework->y == firework->y1)break;
        }while((*firework->y)--);
        do{
            LCD_Circle(s1);
            rt_thread_mdelay(6 * i + 20);
            s1->r++;
        }while(s1->r < r1);
        LCD_Fill_Circle(s1, color_t);
        rt_thread_mdelay(300 * i);
    }
}

static void firework_thread_entry2(void *f0)
{
    struct my_image  *image1   = image_init(40, 7, Square);
    get_image(image1, (uint16_t *)gImage_shoot);
    struct my_circle *s1 = image_init(1, 18, Circle);       //烟花
    struct fireWorks *firework = (struct fireWorks *)rt_malloc(sizeof(struct fireWorks)); //烟花弹
    firework->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->firework = image_init(3, 7, Square);
    firework->firework->x = firework->x;
    firework->firework->y = firework->y;
    s1->x = firework->x;
    s1->y = firework->y;
    uint16_t r1;
    int i;
    while(1)
    {
        i = rand() % 10;
        firework_init(firework, image1, s1, f0, i);
        r1 = rand() % 8 + 10;
        do{
            LCD_Image(firework->firework);
            rt_thread_mdelay(2 * i + 5);
            LCD_Fill(*firework->x, *firework->y, firework->firework->width, firework->firework->high, color_t);
            if(*firework->y == firework->y1)break;
        }while((*firework->y)--);
        do{
            LCD_Circle(s1);
            rt_thread_mdelay(4 * i + 20);
            s1->r++;
        }while(s1->r < r1);
        LCD_Fill_Circle(s1, color_t);
        rt_thread_mdelay(200 * i);
    }
}


static void firework_thread_entry3(void *f0)
{
    struct my_image  *image1   = image_init(40, 7, Square);
    get_image(image1, (uint16_t *)gImage_shoot);
    struct my_circle *s1 = image_init(1, 18, Circle);       //烟花
    struct fireWorks *firework = (struct fireWorks *)rt_malloc(sizeof(struct fireWorks)); //烟花弹
    firework->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->firework = image_init(3, 7, Square);
    firework->firework->x = firework->x;
    firework->firework->y = firework->y;
    s1->x = firework->x;
    s1->y = firework->y;
    uint16_t r1;
    int i;
    while(1)
    {
        i = rand() % 8;
        firework_init(firework, image1, s1, f0, i);
        r1 = rand() % 8 + 10;
        do{
            LCD_Image(firework->firework);
            rt_thread_mdelay(15 * i + 5);
            LCD_Fill(*firework->x, *firework->y, firework->firework->width, firework->firework->high, color_t);
            if(*firework->y == firework->y1)break;
        }while((*firework->y)--);
        do{
            LCD_Circle(s1);
            rt_thread_mdelay(20 * i + 20);
            s1->r++;
        }while(s1->r < r1);
        LCD_Fill_Circle(s1, color_t);
        rt_thread_mdelay(350 * i);
    }
}

static void firework_thread_entry4(void *f0)
{
    struct my_image  *image1   = image_init(40, 7, Square);
    get_image(image1, (uint16_t *)gImage_shoot);
    struct my_circle *s1 = image_init(1, 18, Circle);       //烟花
    struct fireWorks *firework = (struct fireWorks *)rt_malloc(sizeof(struct fireWorks)); //烟花弹
    firework->x = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->y = (uint16_t *)rt_malloc(sizeof(uint16_t));
    firework->firework = image_init(3, 7, Square);
    firework->firework->x = firework->x;
    firework->firework->y = firework->y;
    s1->x = firework->x;
    s1->y = firework->y;
    uint16_t r1;
    int i;
    while(1)
    {
        i = rand() % 8;
        firework_init(firework, image1, s1, f0, i);
        r1 = rand() % 8 + 10;
        do{
            LCD_Image(firework->firework);
            rt_thread_mdelay(5 * i + 5);
            LCD_Fill(*firework->x, *firework->y, firework->firework->width, firework->firework->high, color_t);
            if(*firework->y == firework->y1)break;
        }while((*firework->y)--);
        do{
            LCD_Circle(s1);
            rt_thread_mdelay(10 * i + 20);
            s1->r++;
        }while(s1->r < r1);
        LCD_Fill_Circle(s1, color_t);
        //rt_kprintf("color_addr = %x\r\n", &color_t);
        //rt_kprintf("color = %d\r\n", color_t);

        rt_thread_mdelay(250 * i);
    }
}

static int firework_Init(void)
{

    struct my_image  *f0 = image_init(36, 144, Square);
    get_image(f0, (uint16_t *)gImage_flower0);
#if 1
    rt_thread_t firework_thread = rt_thread_create("firework", firework_thread_entry, f0, 336, 24, 10);
    if (firework_thread != RT_NULL)
    {
        rt_thread_startup(firework_thread);
    }
#endif
#if 1
    rt_thread_t firework_thread1 = rt_thread_create("firework1", firework_thread_entry1, f0, 336, 24, 10);
    if (firework_thread1 != RT_NULL)
    {
        rt_thread_startup(firework_thread1);
    }
#endif
#if 1
    rt_thread_t firework_thread2 = rt_thread_create("firework2", firework_thread_entry2, f0, 336, 24, 10);
    if (firework_thread2 != RT_NULL)
    {
        rt_thread_startup(firework_thread2);
    }
#endif
#if 1
    rt_thread_t firework_thread3 = rt_thread_create("firework3", firework_thread_entry3, f0, 336, 24, 10);
    if (firework_thread3 != RT_NULL)
    {
        rt_thread_startup(firework_thread3);
    }
#endif
#if 1
    rt_thread_t firework_thread4 = rt_thread_create("firework4", firework_thread_entry4, f0, 336, 24, 10);
    if (firework_thread4 != RT_NULL)
    {
        rt_thread_startup(firework_thread4);
    }
#endif
    LCD_Fill(0, 0, 160, 130, 0x00);
    return 0;
}

INIT_APP_EXPORT(firework_Init);
#endif
