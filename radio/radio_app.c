/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-13     Rick       the first version
 */
#include "rtthread.h"
#include "main.h"
#include "radio.h"
#include "radio_app.h"
#include "radio_encoder.h"

#define DBG_TAG "RADIO_APP"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

CRC_HandleTypeDef hcrc;
RadioEvents_t RadioEvents;
rt_thread_t Subghz_t = RT_NULL;

uint8_t rx_convert_buf[128];
uint8_t tx_convert_buf[128];

extern uint32_t Target_ID;

static void OnTxDone(void)
{
    //LOG_D("OnTxDone\r\n");

    Radio.SetMaxPayloadLength(MODEM_FSK, Frame_Size);
    Radio.Rx(0);
}
void RF_Send(char *payload,int size)
{
    rt_memset(tx_convert_buf, 0, sizeof(tx_convert_buf));
    uint16_t pkt_size = size + 1;//add len
    uint16_t send_size = size + 3;//add len,crc_H,crc_L
    rt_memcpy(&tx_convert_buf[1],payload,size);
    tx_convert_buf[0] = pkt_size;
    uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)tx_convert_buf, pkt_size) ^ 0xffff;
    tx_convert_buf[pkt_size+1] = (calc_crc&0xff00)>>8;
    tx_convert_buf[pkt_size] = calc_crc & 0xff;
    Radio.Send(tx_convert_buf, send_size);
}
void send_test(void)
{
    char buf[]={0x7b,0x31,0x30,0x30,0x30,0x30,0x32,0x32,0x32,0x2c,0x32,0x38,0x30,0x30,0x38,0x33,0x30,0x34,0x2c,0x30,0x36,0x34,0x2c,0x30,0x35,0x2c,0x30,0x7d,0x31,0x31,0x0d,0x0a};
    RF_Send(buf,32);
}

MSH_CMD_EXPORT(send_test,send_test);


void S(void)
{
    rf_433_Enqueue(Target_ID,3,1);
}

MSH_CMD_EXPORT(S,send_t);

void O(void)
{
    rf_433_Enqueue(Target_ID,2,1);
}

MSH_CMD_EXPORT(O,open);

void C(void)
{
    rf_433_Enqueue(Target_ID,2,0);
}
MSH_CMD_EXPORT(C,Close);

void H(void)
{
    heart_request();
}
MSH_CMD_EXPORT(H,heart);

static void OnRxDone(uint8_t *src_payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
    char rx_crc_convert_buf[8] = { 0 };
    CRC_Config_init();
    uint16_t real_size = src_payload[0];
    if(real_size != 36)
    {
        return;
    }
    LOG_D("real_size is %d,Recv size is %d\r\n",real_size,size);
    rt_memset(rx_convert_buf, 0, sizeof(rx_convert_buf));
    for(uint16_t i=0;i<real_size;i++)
    {
        rx_convert_buf[i] = src_payload[i];
    }
    for(uint16_t i=real_size;i<(real_size+2);i++)
    {
        rx_crc_convert_buf[i] = src_payload[i];
    }
    uint32_t src_crc = rx_crc_convert_buf[real_size] | rx_crc_convert_buf[real_size + 1]<<8;
    uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)rx_convert_buf, real_size) ^ 0xffff;
    LOG_D("src_crc %04X,Recv calc_crc %04X\r\n",src_crc,calc_crc);
    if(calc_crc == src_crc)
    {
        LOG_D("RSSI %d,Recv Size %d,Recv Payload is %s\r\n",rssi,real_size,rx_convert_buf);
        rf433_rx_callback(rssi,rx_convert_buf,real_size);
    }
    else
    {
        LOG_E("RSSI %d,Recv Size %d,Recv Payload is %s\r\n",rssi,real_size,rx_convert_buf);
    }

}
static void OnTxTimeout(void)
{
    LOG_W("OnTxTimeout\r\n");
}
static void OnRxTimeout(void)
{
    LOG_W("OnRxTimeout\r\n");
}
static void OnRxError(void)
{
    LOG_W("OnRxError\r\n");
}

void CRC_Config_init(void)
{
    /* CRC initialization */
    hcrc.Instance = CRC;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
    hcrc.Init.GeneratingPolynomial = 0x8005;
    hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
    hcrc.Init.InitValue = 0xFFFF;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    if (HAL_CRC_Init(&hcrc) != HAL_OK)
    {
        Error_Handler();
    }
}

void SubghzApp_Callback(void *parameter)
{
    CRC_Config_init();
    /* Radio initialization */

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init(&RadioEvents);

    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetPublicNetwork( false);

    Radio.SetTxConfig(MODEM_LORA, LORA_TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
            LORA_SPREADING_FACTOR, LORA_CODINGRATE,
            LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON_ENABLE,
                    false ,0, 0, LORA_IQ_INVERSION_ON_DISABLE, TX_TIMEOUT_VALUE);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
            LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
            LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON_ENABLE, 0, false,
                    0, 0, LORA_IQ_INVERSION_ON_DISABLE, true);
    Radio.SetMaxPayloadLength(MODEM_LORA, Frame_Size);
    Radio.Rx(0);
    while(1)
    {
        rt_thread_mdelay(10);
    }
}
void RF_Init(void)
{
    Subghz_t = rt_thread_create("Subghz_t", SubghzApp_Callback, RT_NULL, 2048, 10, 10);
    if(Subghz_t != RT_NULL)
    {
        rt_thread_startup(Subghz_t);
    }
    RadioQueue_Init();
}
