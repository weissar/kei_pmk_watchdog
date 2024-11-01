#include "stm_watchddog.h"

#ifndef STM32F4
#error WDG configuration only for STM32F4 family
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif  // NULL

#define WDG_MAGIC_CONSTANT    0xDEADBEEF

// https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html#SEC6
#if 1
// https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html
// __attribute__ ((section ("wdg_ram_data"))) static uint32_t _wdg_data_area[2];
static uint32_t _wdg_data_area[2] __attribute__ ((section (".wdg_ram_data")));
#else
extern uint8_t _swdg_ram_data;
static uint32_t *_wdg_data_area = (uint32_t *)&_swdg_ram_data;
#endif

bool STM_IWDG_Init(int timeMs)
{
  IWDG->KR = 0x5555;                  // unlock write protect

  while (IWDG->SR & IWDG_SR_PVU)      // wait for PR executed
    ;
  IWDG->PR = 3 << IWDG_PR_PR_Pos;     // 011: divider /32 ... cca 1 ms

  while (IWDG->SR & IWDG_SR_RVU)      // wait for RLR executed
    ;
  IWDG->RLR = timeMs - 1;             // xx msec = xx steps

  IWDG->KR = 0xCCCC;                  // start "watching"

  _wdg_data_area[0] = WDG_MAGIC_CONSTANT;
  _wdg_data_area[1] = 0;
  return true;
}

bool STM_IWDG_Reset(void)
{
  IWDG->KR = 0xAAAA;            // WDG restart
  return true;                  //TODO check if IWDG is running
}

bool STM_IWDG_SetReason(int reason)
{
  _wdg_data_area[1] = reason;
  return true;
}

int STM_WDG_GetReason(void)
{
  return _wdg_data_area[1];
}

uint32_t STM_WDG_GetMagic(void)
{
  return _wdg_data_area[0];
}
