/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-02     Tobby       the first version
 */
#ifndef PERIPHERAL_PIN_CONFIG_H_
#define PERIPHERAL_PIN_CONFIG_H_

/*
 * RF
 */
#define RF_SW1_PIN                      6
#define RF_SW2_PIN                      7
#define TCXO_PWR_PIN                    16

/*
 * RELAY
 */
#define RELAY1                       rt_pin_get("PA.15")
#define RELAY2                       rt_pin_get("PB.2")

/*
 * LED
 */
#define LED_G_PIN                       rt_pin_get("PB.4")
/*
 * KEY
 */
#define KEY3                            rt_pin_get("PB.5")

/*
 * SWITCH
 */
#define SWITCH1                         rt_pin_get("PB.6")
#define SWITCH2                         rt_pin_get("PB.7")

/*
 * TRC_ON
 */
#define TRC_ON                          rt_pin_get("PB.8")
#endif /* PERIPHERAL_PIN_CONFIG_H_ */
