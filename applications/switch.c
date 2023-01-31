/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-01     tobby       the first version
 */
#include <rtthread.h>
#include <agile_led.h>
#include <stdlib.h>
#include "Pin_Config.h"
#include "led.h"
#include "heart.h"

#define DBG_TAG "PSI"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


rt_thread_t Switch_thread = RT_NULL;
uint8_t SW1_Status = 0, SW2_Status = 0;
uint8_t SW1_Flag = 0, SW2_Flag = 0;


uint8_t SW_Lost;
extern enum Device_Status Now_Status;

void SW1_open(void)
{
    if (SW1_Status == 0 && Now_Status == Normal)
    {
        SW1_Status = 1;
        transmitter_on();
        SW1_upload(1);
        LOG_I("Detect SW1 is open\r\n");
    }
}
MSH_CMD_EXPORT(SW1_open,SW1_open);

void SW1_close(void)
{
    if (SW1_Status == 1 && Now_Status == Normal)
    {
        SW1_Status = 0;
        transmitter_on();
        SW1_upload(0);
        LOG_I("Detect SW1 is close\r\n");
    }
}
MSH_CMD_EXPORT(SW1_close,SW1_close);

void SW2_open(void)
{
    if (SW2_Status == 0 && Now_Status == Normal)
    {
        SW2_Status = 1;
        transmitter_on();
        SW2_upload(1);
        LOG_I("Detect SW2 is open\r\n");
    }
}
MSH_CMD_EXPORT(SW2_open,SW2_open);

void SW2_close(void)
{
    if (SW2_Status == 1 && Now_Status == Normal)
    {
        SW2_Status = 0;
        transmitter_on();
        SW2_upload(0);
        LOG_I("Detect SW2 is close\r\n");
    }
}
MSH_CMD_EXPORT(SW2_close,SW2_close);

void Switch_work(uint8_t SW1_Level,uint8_t SW2_Level)
{
    if (SW1_Level == 0)
    {
        SW1_open();
        SW1_Flag = 1;
    }
    else
    {
        SW1_close();
        SW1_Flag = 0;
    }
    if (SW2_Level == 0)
    {
        SW2_open();
        SW2_Flag = 1;
    }
    else
    {
        SW2_close();
        SW2_Flag = 0;
    }
}

void SwitchScan_Callback(void *parameter)
{
    uint8_t SW1_Level = 0;
    uint8_t SW2_Level = 0;
    rt_pin_mode(SWITCH1, PIN_MODE_INPUT);
    rt_pin_mode(SWITCH2, PIN_MODE_INPUT);

    while (1)
    {
        /****SWITCH1 SCANF*********/
        if (rt_pin_read(SWITCH1) == 0)
        {
            rt_thread_mdelay(100);
            if (rt_pin_read(SWITCH1) == 0)
            {
                SW1_Level = 0;
            }
        }
        else
        {
            rt_thread_mdelay(100);
            if (rt_pin_read(SWITCH1) != 0)
            {
                SW1_Level = 1;
            }
        }
        /****SWITCH2 SCANF*********/
        if (rt_pin_read(SWITCH2) == 0)
        {
            rt_thread_mdelay(100);
            if (rt_pin_read(SWITCH2) == 0)
            {
                SW2_Level = 0;
            }
        }
        else
        {
            rt_thread_mdelay(100);
            if (rt_pin_read(SWITCH2) != 0)
            {
                SW2_Level = 1;
            }
        }
        /****SWITCH1 2 WORK*********/
        Switch_work(SW1_Level,SW2_Level);
        rt_thread_mdelay(100);
    }
}

void Switch_init(void)
{
    Switch_thread = rt_thread_create("Switch_thread", SwitchScan_Callback, RT_NULL, 1024, 11, 10);
    if(Switch_thread!=RT_NULL)
    {
        rt_thread_startup(Switch_thread);
    }
}
