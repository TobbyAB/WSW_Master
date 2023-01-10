/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-28     Tobby       the first version
 */
#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32wl_onchip_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32wl_onchip_flash,                                           \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                                              \
{                                                                                                                   \
    {FAL_PART_MAGIC_WROD,        "app", "onchip_flash",                                    0,       240 * 1024, 0},\
    {FAL_PART_MAGIC_WROD,      "easyflash", "onchip_flash",                           240 * 1024,       256 * 1024, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */
#endif /* _FAL_CFG_H_ */
