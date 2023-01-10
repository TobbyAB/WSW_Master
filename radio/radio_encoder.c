/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <stdio.h>
#include "radio_encoder.h"
#include "radio_app.h"

#define DBG_TAG "RADIO_ENCODER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_mq_t rf_en_mq;
rt_thread_t rf_encode_t = RT_NULL;

uint32_t Self_Id = 0;
uint32_t Target_ID =0;
uint32_t Self_Default_Id = 10000111;



void rf_433_Enqueue(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t *buf = rt_malloc(50);
    sprintf((char *)buf,"Master{%08ld,%08ld,%02d,%02d,XXX}",Self_Id,target_id,control,value);
    rt_mq_send(rf_en_mq, buf, 39);
    rt_free(buf);
}

void rf_433_Urgent_Enqueue(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t *buf = rt_malloc(50);
    sprintf((char *)buf,"Master{%08ld,%08ld,%02d,%02d,XXX}",Self_Id,target_id,control,value);
    rt_mq_urgent(rf_en_mq, buf, 39);
    rt_free(buf);
}

void rf_encode_entry(void *paramaeter)
{
    while (1)
    {
        char buf[64] = { 0 };
        if (rt_mq_recv(rf_en_mq, buf, sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            //rt_kprintf("rt_strlen buf is %d\r\n",rt_strlen(buf));

            RF_Send(buf, rt_strlen(buf));

            rt_thread_mdelay(300);
        }
        rt_thread_mdelay(50);
    }
}

void RadioQueue_Init(void)
{
//    int *p;
//    p=(int *)(0x0800FFF0);//这就是已知的地址，要强制类型转换
//    Self_Id = *p;//从Flash加载ID
    Self_Id = 0; //加载为空值
    if (Self_Id == 0xFFFFFFFF || Self_Id == 0)
    {
        Self_Id = Self_Default_Id;
    }
    LOG_I("Self ID is %d\r\n", Self_Id);

    rf_en_mq = rt_mq_create("rf_en_mq", 47, 6, RT_IPC_FLAG_PRIO);
    rf_encode_t = rt_thread_create("radio_send", rf_encode_entry, RT_NULL, 1024, 9, 10);
    if (rf_encode_t)rt_thread_startup(rf_encode_t);
}
