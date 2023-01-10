#include <agile_led.h>
#include <stdlib.h>
#include "led.h"
#include "pin_config.h"
#include <agile_led.h>
#include "rtdef.h"

static agile_led_t *LED_G = RT_NULL;

#define DBG_TAG "LED"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void led_Init(void)
{
    if (LED_G == RT_NULL)
    {
        LED_G = agile_led_create(LED_G_PIN, PIN_LOW, "200,200", -1);
    }
}

void rf_led_resume(agile_led_t *led)
{
    agile_led_set_compelete_callback(LED_G, RT_NULL);
    agile_led_set_light_mode(LED_G, "200,0", -1);
    agile_led_start(LED_G);
}

void SW_led(uint8_t type)
{
    switch (type)
    {
    case 0: //初始化失败
        agile_led_off(LED_G);
        agile_led_stop(LED_G);
        break;
    case 1: //慢闪
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_G, "1000,1000", -1);
        agile_led_start(LED_G);
        break;
    case 2: //已经连接上从机
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_G, "200,0", -1);
        agile_led_start(LED_G);
        break;
    case 3: //WLE55 发送/接收
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_G, "10,100",1);
        agile_led_set_compelete_callback(LED_G,rf_led_resume);
        agile_led_start(LED_G);
        break;
    case 4: //快闪
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_G, "100,100", -1);
        agile_led_start(LED_G);
        break;
    default:
        break;
    }
}

void LED_stop(void)
{
    SW_led(0);
}
MSH_CMD_EXPORT(LED_stop,LED_stop);


void LED_Slow_flash(void)
{
    SW_led(1);
}
MSH_CMD_EXPORT(LED_Slow_flash,LED_Slow_flash);

void LED_Grean(void)
{
    SW_led(2);
}
MSH_CMD_EXPORT(LED_Grean,LED_Grean);

void LED_G_ONE(void)
{
    SW_led(3);
}
MSH_CMD_EXPORT(LED_G_ONE,LED_G_ONE);

void transmitter_on(void)
{
    agile_led_set_light_mode(LED_G,"300,100",1);
    agile_led_start(LED_G);
}

void G(void)
{
    agile_led_set_light_mode(LED_G,"300,100",1);
    agile_led_start(LED_G);
}

MSH_CMD_EXPORT(G,LED_one_light);


void learn_success(void)
{
    agile_led_stop(LED_G);
    agile_led_set_light_mode(LED_G, "200,200", 5);
    agile_led_start(LED_G);
}

MSH_CMD_EXPORT(learn_success,learn_success);
void learn_fail(void)
{
    LOG_W("Master learn fail\r\n");
    agile_led_set_light_mode(LED_G, "50,50,200,200", 3);
    agile_led_start(LED_G);
}
MSH_CMD_EXPORT(learn_fail,learn_fail);
INIT_APP_EXPORT(led_Init);

