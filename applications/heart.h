/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-01     tobby       the first version
 */
#ifndef APPLICATIONS_HEART_H_
#define APPLICATIONS_HEART_H_

typedef struct
{
    uint16_t freq;
    uint8_t retry;
    uint8_t alive;
    uint8_t received;
    uint8_t test;
    uint8_t testreceived;
    int rssi;
    uint8_t rssi_level;
    uint8_t startup;
}rf_info;

enum Device_Status
{
    Normal = 0, Lost, Learn
};

#endif /* APPLICATIONS_HEART_H_ */
