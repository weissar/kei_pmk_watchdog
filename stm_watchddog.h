/*
 * stm_watchddog.h
 *
 *  Created on: Oct 3, 2024
 *      Author: Weissar
 */

#ifndef STM_WATCHDDOG_H_
#define STM_WATCHDDOG_H_

#include "stm_core.h"

bool STM_IWDG_Init(int timeMs);
bool STM_IWDG_Reset(void);
bool STM_IWDG_SetReason(int reason);
int STM_WDG_GetReason(void);
uint32_t STM_WDG_GetMagic(void);

#endif /* STM_WATCHDDOG_H_ */
