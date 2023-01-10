/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-15     Rick       the first version
 */
#ifndef RADIO_RADIO_APP_H_
#define RADIO_RADIO_APP_H_

#define USE_MODEM_LORA  0
#define USE_MODEM_FSK   1
/*
 * Packet Config
 */
#define Frame_Size                                  47
/*
 * FSK Config
 */
#define FSK_FDEV                                    1660
#define FSK_DATARATE                                4800
#define FSK_BANDWIDTH                               7200
#define FSK_PREAMBLE_LENGTH                         4
#define FSK_FIX_LENGTH_PAYLOAD_ON                   true
#define FSK_AFC_BANDWIDTH                           870
/*
 * Radio Config
 */
#define RF_FREQUENCY                                434500000
#define TX_OUTPUT_POWER                             10
#define RX_TIMEOUT_VALUE                            0
#define TX_TIMEOUT_VALUE                            3000

#define LORA_TX_OUTPUT_POWER                        10  //14    // dBm

#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
//  1: 250 kHz,
//  2: 500 kHz,
//  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
//  2: 4/6,
//  3: 4/7,
//  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON_DISABLE          false
#define LORA_IQ_INVERSION_ON_DISABLE                false

void RF_Init(void);
void RF_Send(char *payload,int size);



#endif /* RADIO_RADIO_APP_H_ */
