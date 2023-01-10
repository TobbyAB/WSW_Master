/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-09     Tobby       the first version
 */
#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "led.h"


#define DBG_TAG "trc"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

void TRC_Init(void)
{
    rt_pin_mode(TRC_ON,PIN_MODE_OUTPUT);
    rt_pin_write(TRC_ON,1);
    LOG_I("TRC_Init Success\r\n");
}
