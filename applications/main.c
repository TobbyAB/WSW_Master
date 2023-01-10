/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-01     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <drivers/pin.h>
#include "pin_config.h"
#include "main.h"

int main(void)
{
    fal_init();
    if (easyflash_init() == 0)
    {
        Flash_Get_Boot();
    }
    int count = 1;
    TRC_Init();
    RF_Init();
    button_init();
    Relay_Init();
    Switch_init();
    heart_init();
    while (1)
    {
        rt_thread_mdelay(100);
    }
    return RT_EOK;
}

