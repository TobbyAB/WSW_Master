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
#include "radio_app.h"
#include "radio_decoder.h"
#include "heart.h"

#define DBG_TAG "RADIO_DECODER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint32_t Self_Id;
extern rf_info info_433;
extern uint32_t Target_ID;
extern uint8_t Learn_Flag;

void Solve_433(int rssi, uint8_t *rx_buffer, uint8_t rx_len)
{
    Message Rx_message;
    LOG_D("Solve initial letter is %c\r\n",rx_buffer[1]);
    if (rx_buffer[1] == 'S')
    {
        sscanf((const char *) &rx_buffer[1], "Slaver{%08ld,%08ld,%02d,%02d,XXX}", &Rx_message.From_ID,
                &Rx_message.Target_ID, &Rx_message.Command, &Rx_message.Data);
        LOG_D("Target_ID  is %08ld \r\n",Rx_message.Target_ID);
        if (Rx_message.Target_ID == Self_Id && Rx_message.From_ID == Target_ID)
        {
            info_433.received = 1;
            info_433.rssi = rssi;
            SW_led(3);
            switch (Rx_message.Command)
            {
            case 0: //心跳
                if (Rx_message.Data)
                {
                    LOG_I("RF 433 heart is acknowledge receipt from Slaver\r\n");
                }
                break;
            case 1: //按钮
                info_433.testreceived = 1;
                LOG_I("RF 433 key is acknowledge receipt from Slaver\r\n");
                break;
            case 2: //SWITCH1
                LOG_I("RF 433 SWITCH1 Control %d is acknowledge receipt from Slaver\r\n", Rx_message.Data);
                break;
            case 3: //SWITCH2
                LOG_I("RF 433 SWITCH2 Control %d is acknowledge receipt from Slaver\r\n", Rx_message.Data);
                break;
            default:
                break;
            }
        }
        else if (Rx_message.Target_ID == 99999999)
        {
            if (Learn_Flag)
            {
                if (Rx_message.Command == 4)
                {
                    Flash_Slaver_Change(Rx_message.From_ID);
                    Target_ID = Flash_Get_Slaver();
                    rf_433_Enqueue(Target_ID,4,1);
                    LOG_I("Successful pairing, Slaver ID of the master is %d ",Target_ID);
                    rt_thread_mdelay(1000);
                    connect_restart();
                }
            }
        }
    }
}

void rf433_rx_callback(int rssi, uint8_t *rx_buffer, uint8_t rx_len)
{
    switch (rx_buffer[1])
    {
    case 'S':
        Solve_433(rssi, rx_buffer, rx_len);
        break;
    }
}

