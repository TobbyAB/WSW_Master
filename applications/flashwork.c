/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-01     tobby       the first version
 */

#include "rtthread.h"
#include "easyflash.h"

#define DBG_TAG "flashwork"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

char read_value_temp[64]={0};
extern uint32_t Target_ID;

void test_env(void)
{
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[11] = { 0 };
    /* 获 得 启 动 次 数 的 值 */
    c_old_boot_times = ef_get_env("boot_times");
    /* 如 果 获 取 失 败 */
    if (c_old_boot_times == RT_NULL)
        c_old_boot_times[0] = '0';
    i_boot_times = atol(c_old_boot_times);
    /* 启 动 次 数+1 */
    i_boot_times++;
    rt_kprintf("===============================================\n");
    rt_kprintf("The system now boot %d times\n", i_boot_times);
    rt_kprintf("===============================================\n");
    /* 数 字 转 字 符 串 */
    sprintf(c_new_boot_times, "%d", i_boot_times);
    /* 保 存 开 机 次 数 的 值 */
    ef_set_env("boot_times", c_new_boot_times);
    ef_save_env();
}

void Flash_Boot_Change(uint32_t value)
{
    const char *keybuf="boot_times";
    char *Temp_ValueBuf = rt_malloc(64);
    sprintf(Temp_ValueBuf, "%ld", value);
    ef_set_env(keybuf, Temp_ValueBuf);
    ef_save_env();
    rt_free(Temp_ValueBuf);
    LOG_I("Writing %ld to key %s\r\n", value,keybuf);
}

uint32_t Flash_Get_Boot(void)
{
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times;
    /* 获 得 启 动 次 数 的 值 */
    c_old_boot_times = ef_get_env("boot_times");
    /* 如 果 获 取 失 败 */
    if (c_old_boot_times == RT_NULL)
        c_old_boot_times[0] = '0';
    i_boot_times = atol(c_old_boot_times);
    /* 启 动 次 数+1 */
    i_boot_times++;
    rt_kprintf("===============================================\n");
    rt_kprintf("The system now boot %d times\n", i_boot_times);
    rt_kprintf("===============================================\n");
    Flash_Boot_Change(i_boot_times);
}


void Flash_Slaver_Change(uint32_t value)
{
    const char *keybuf="Slaver_num";
    char *Temp_ValueBuf = rt_malloc(64);
    sprintf(Temp_ValueBuf, "%ld", value);
    ef_set_env(keybuf, Temp_ValueBuf);
    ef_save_env();
    rt_free(Temp_ValueBuf);
    LOG_I("Writing %ld to key %s\r\n", value,keybuf);
}

uint32_t Flash_Get_Slaver(void)
{
    uint32_t Slaver_number = NULL;
    char *Slaver_num_val;
    /* 获 得 从机 的 值 */
    Slaver_num_val = ef_get_env("Slaver_num");
    /* 如 果 获 取 失 败 */
    if (Slaver_num_val == RT_NULL)
        Slaver_num_val[0] = '0';
    Slaver_number = atol(Slaver_num_val);
    rt_kprintf("===============================================\n");
    rt_kprintf("Flash Get Slaver Number %d \r\n", Slaver_number);
    rt_kprintf("===============================================\n");
    return Slaver_number;
}

void M(void)
{
    Flash_Slaver_Change(20000005);
    Target_ID = Flash_Get_Slaver();
}
MSH_CMD_EXPORT(M,Flash_Slaver_Change);

void B(void)
{
    Flash_Slaver_Change(20000222);
    Target_ID = Flash_Get_Slaver();
}
MSH_CMD_EXPORT(B,Flash_Slaver_Change);

void N(void)
{
    Flash_Get_Slaver();
}

MSH_CMD_EXPORT(N,Flash_Get_Slaver);

void F(void)
{
    Flash_Boot_Change(30);
}
MSH_CMD_EXPORT(F,F);
