/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     Rick       the first version
 */
#include <rtthread.h>
#include <stdint.h>
#include "pin_config.h"
#include "agile_button.h"

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

enum Device_Status
{
    Normal = 0, Lost, Learn
};

enum Device_Status Now_Status = Lost;

uint8_t Learn_Flag = 0;
uint8_t Long_Flag = 0;
static agile_btn_t *test_btn = RT_NULL;

rt_timer_t Learn_Timer = RT_NULL;

void test_single_callback(agile_btn_t *btn)
{
    if (Long_Flag)
    {
        Long_Flag = 0;
    }
    else if(Now_Status != Learn)
    {
        heart_single();
        LOG_I("heart single\r\n");
    }
}

void test_hold_callback(agile_btn_t *btn)
{
    Long_Flag = 1;
    if (Now_Status == Normal || Now_Status == Lost)
    {
        heart_stop();
        Now_Status = Learn;
        Learn_Flag = 1;
        rt_timer_start(Learn_Timer);
        LOG_D("Now Start Learn\r\n");
    }
    else if (Now_Status == Learn)
    {
        Learn_Flag = 0;
        Now_Status = Lost;
        rt_timer_stop(Learn_Timer);
        heart_restart();
        LOG_D("Start Learn timer is stop\r\n");
    }
}

void Learn_Timer_Callback(void *parameter)
{
    LOG_D("Learn timer is Timeout\r\n");
    Learn_Flag = 0;
    heart_restart();
}

void button_init(void)
{
    test_btn = agile_btn_create(KEY3, PIN_LOW, PIN_MODE_INPUT_PULLUP);
    agile_btn_set_elimination_time(test_btn, 50);
    agile_btn_set_hold_cycle_time(test_btn, 3000);
    agile_btn_set_event_cb(test_btn, BTN_CLICK_EVENT, test_single_callback);
    agile_btn_set_event_cb(test_btn, BTN_HOLD_EVENT, test_hold_callback);
    agile_btn_start(test_btn);
    Learn_Timer = rt_timer_create("Learn_Timer", Learn_Timer_Callback, RT_NULL, 30 * 1000,
            RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    LOG_I("button_init Success\r\n");
}
