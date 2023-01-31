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
#include <stdio.h>
#include <stdlib.h>
#include "heart.h"
#include "Radio_Encoder.h"

#define DBG_TAG "heart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define send 0
#define check 1
#define randtime_7 2
#define randtime_8 3

rt_thread_t heart_t = RT_NULL;
rt_thread_t buttontest_t = RT_NULL;

rt_timer_t connect_timer = RT_NULL;
rt_timer_t heart_timer = RT_NULL;

rt_sem_t connect_sem = RT_NULL;
rt_sem_t heart_sem = RT_NULL;
rt_sem_t button_sem = RT_NULL;
rt_mutex_t rf_lock = RT_NULL;

rf_info info_433;

uint8_t heart_mode;
uint32_t rand_time;
uint32_t remain_time;

extern enum Device_Status Now_Status;
extern uint32_t Target_ID;
extern uint8_t SW1_Status, SW2_Status;
extern uint8_t SW1_Flag, SW2_Flag;

void heart_single(void)
{
    heart_request();
}

void SW1_upload(uint8_t value)
{
    if (Now_Status == Normal)
    {
        rf_433_Urgent_Enqueue(Target_ID, 2, value);
    }
}

void SW2_upload(uint8_t value)
{
    if (Now_Status == Normal)
    {
        rf_433_Urgent_Enqueue(Target_ID, 3, value);
    }
}

void radio_refresh(rf_info *temp)
{
    temp->retry = 0;
    temp->alive = 1;
    Now_Status = Normal;
    LOG_I("RF %d is online,Alive value is %d\r\n", temp->freq, temp->alive);
}
void rf_offline(rf_info *temp)
{
    if (temp->alive)
    {
        temp->alive = 0;
    }
    if (Now_Status == Normal)
    {
        Now_Status = Lost;
    }
    LOG_W("RF %d is offline,Alive value is %d\r\n", temp->freq, temp->alive);
}
void heart_request(void)
{
    info_433.received = 0;

    if (SW1_Flag == 0 && SW2_Flag == 0)
    {
        rf_433_Urgent_Enqueue(Target_ID, 0, 0);
    }
    else if (SW1_Flag == 1 && SW2_Flag == 0)
    {
        rf_433_Urgent_Enqueue(Target_ID, 0, 1);
    }
    else if (SW1_Flag == 0 && SW2_Flag == 1)
    {
        rf_433_Urgent_Enqueue(Target_ID, 0, 2);
    }
    else if (SW1_Flag == 1 && SW2_Flag == 1)
    {
        rf_433_Urgent_Enqueue(Target_ID, 0, 3);
    }
}
void connect_timer_callback(void *parameter)
{
    rt_sem_release(connect_sem);
}
void heart_timer_callback(void *parameter)
{
    rt_sem_release(heart_sem);
}

void connect_restart(void *parameter)
{
    rt_sem_release(connect_sem);
}
uint32_t get_srand_time(uint32_t min, uint32_t max)
{
    uint32_t value;
    srand(rt_tick_get());
    value = rand() % (max * 10) + min * 10;
    if (value == 0)
        value = 1;
    LOG_D("Rand Time is %d\r\n",value*100);
    return value * 100;
}

void heart_time_start(uint32_t time)
{
    rt_timer_control(heart_timer, RT_TIMER_CTRL_SET_TIME, &time);
    rt_timer_start(heart_timer);
}

void heart_callback(void *parameter)
{
    rf_info *info_temp = rt_malloc(sizeof(rf_info));
    info_temp = &info_433;
    while (1)
    {
        if (rt_sem_take(connect_sem, 100) == RT_EOK)
        {
            heart_request();
            rt_thread_mdelay(2000);
            LOG_I("rf_connected is %d\r\n", info_433.received);
            if (info_433.received)
            {
                LOG_I("rf_connected success\r\n");
                rt_timer_stop(connect_timer);
                radio_refresh(&info_433);
                heart_time_start(10000);
                heart_mode = send;
            }
            else
            {
                LOG_W("rf_connected fail\r\n");
            }
        }
        if (rt_sem_take(heart_sem, 100) == RT_EOK)
        {
            switch (heart_mode)
            {
            case send: //发送
                heart_request();
                heart_time_start(2000);
                heart_mode = check;
                rt_mutex_trytake(rf_lock);
                break;
            case 1: //应答检查
                switch (info_temp->retry)
                {
                case 0:
                    if (info_temp->received)
                    {
                        heart_mode = send;
                        LOG_I("radio_%d first heart success\r\n", info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(27700);
                    }
                    else
                    {
                        heart_mode = randtime_7;
                        LOG_W("radio_%d first heart fail\r\n", info_temp->freq);
                        info_temp->retry++;
                        rt_sem_release(heart_sem);
                    }
                    break;
                case 1:
                    if (info_temp->received)
                    {
                        heart_mode = send;
                        LOG_I("radio_%d second heart success\r\n", info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(27700);
                    }
                    else
                    {
                        heart_mode = randtime_7;
                        LOG_W("radio_%d second heart fail\r\n", info_temp->freq);
                        info_temp->retry++;
                        rt_sem_release(heart_sem);
                    }
                    break;
                case 2:
                    if (info_temp->received)
                    {
                        heart_mode = send;
                        LOG_I("radio_%d third heart success\r\n", info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(27700);
                    }
                    else
                    {
                        heart_mode = randtime_8;
                        LOG_W("radio_%d third heart fail\r\n", info_temp->freq);
                        info_temp->retry++;
                        heart_time_start(1000);
                    }
                    break;
                case 3:
                    if (info_temp->received)
                    {
                        LOG_I("radio_%d final heart success\r\n", info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(27700);
                    }
                    else
                    {
                        LOG_W("radio_%d final heart fail\r\n", info_temp->freq);
                        info_temp->retry = 0;
                        rf_offline(info_temp);
                        SW_led(1);
                        rt_sem_release(heart_sem);
                    }
                    heart_mode = send;
                    break;
                }
                rt_mutex_release(rf_lock);
                break;
            case randtime_7: //第1，2次随机发送等待
                heart_mode = send;
                rand_time = get_srand_time(0, 7);
                heart_time_start(rand_time);
                break;
            case randtime_8: //第3次随机发送等待
                heart_mode = send;
                rand_time = get_srand_time(0, 8);
                heart_time_start(rand_time);
                break;
            }
        }
    }
}

void heart_stop(void)
{
    rt_timer_stop(heart_timer);
    rt_timer_stop(connect_timer);
    SW_led(4); //快闪
}

void heart_restart(void)
{
    rt_timer_start(connect_timer);
    SW_led(1); //快闪
}

void heart_init(void)
{
    info_433.freq = 433;
    Target_ID = Flash_Get_Slaver();
    rf_lock = rt_mutex_create("rf_Lock", RT_IPC_FLAG_PRIO);
    heart_t = rt_thread_create("heart", heart_callback, RT_NULL, 2048, 7, 10);
    connect_sem = rt_sem_create("connect_sem", 0, RT_IPC_FLAG_PRIO);
    connect_timer = rt_timer_create("connect", connect_timer_callback, RT_NULL, 5000,
    RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    heart_sem = rt_sem_create("heart_sem", 0, RT_IPC_FLAG_PRIO);
    heart_timer = rt_timer_create("heart", heart_timer_callback, RT_NULL, 1000,
    RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
    button_sem = rt_sem_create("button_sem", 0, RT_IPC_FLAG_PRIO);
    if (heart_t != RT_NULL)
    {
        rt_thread_startup(heart_t);
    }
    rt_timer_start(connect_timer);
    SW_led(1); //慢闪
}
