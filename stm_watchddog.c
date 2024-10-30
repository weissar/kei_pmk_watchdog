#include "stm_watchddog.h"

#ifndef STM32F4
#error WDG configuration only for STM32F4 family
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif  // NULL

//! configurable !!
// #define USE_WDG_STORE_ESTACK

#define WDG_MAGIC_CONSTANT    0xDEADBEEF

#ifdef USE_WDG_STORE_ESTACK
// Symbols defined in the linker script
extern uint8_t _estack;
extern uint8_t _eram;
extern uint32_t _Min_Stack_Size;
#endif  // USE_WDG_STORE_ESTACK

static uint32_t *_memGapAddress = (void *)0xffffffff;

void _calcGapAddress(void)
{
  if ((uint32_t)_memGapAddress == 0xffffffff)
  {
#ifdef USE_WDG_STORE_ESTACK
    const uint32_t ram_stack_gap = (uint32_t)&_eram - (uint32_t)&_estack;

    if (ram_stack_gap >= 8)
    {
      _memGapAddress = (uint32_t *)((uint32_t)&_eram - 8);
    }
#else
#ifndef STM32F411xE
#error This code is specific for F411, e.g. with RAM size 128kB
#endif

#define _RAM_END  (SRAM_BASE + 128 * 1024)        // 128k RAM

    uint32_t initSP = *(uint32_t *)0;   // SP from vector area
    if (initSP <= (_RAM_END - 8))       // min. 8 bytes from RAM end ?
    {
      _memGapAddress = (uint32_t *)(_RAM_END - 8);
    }
    else
      _memGapAddress = NULL;            // for sure
#endif  // USE_WDG_STORE_ESTACK
  }
}

bool STM_IWDG_Init(int timeMs)
{
  _calcGapAddress();

  IWDG->KR = 0x5555;                  // unlock write protect

  while (IWDG->SR & IWDG_SR_PVU)      // wait for PR executed
    ;
  IWDG->PR = 3 << IWDG_PR_PR_Pos;     // 011: divider /32 ... cca 1 ms

  while (IWDG->SR & IWDG_SR_RVU)      // wait for RLR executed
    ;
  IWDG->RLR = timeMs - 1;             // xx msec = xx steps

  IWDG->KR = 0xCCCC;                  // start "watching"

  if (_memGapAddress != NULL)         // is there "special" area ?
  {
#ifdef USE_WDG_STORE_ESTACK
    uint32_t *xptr = (uint32_t *)(&_eram - 8);

    xptr[0] = WDG_MAGIC_CONSTANT;
    xptr[1] = 0;
#else
    _memGapAddress[0] = WDG_MAGIC_CONSTANT;
    _memGapAddress[1] = 0;
#endif  // USE_WDG_STORE_ESTACK
    return true;
  }
  else
    return false;
}

bool STM_IWDG_Reset(void)
{
  IWDG->KR = 0xAAAA;            // WDG restart
  return true;                  //TODO check if IWDG is running
}

bool STM_IWDG_SetReason(int reason)
{
  // zde se predpoklada uspesny "Init"
  if (_memGapAddress != NULL)
  {
    _memGapAddress[1] = reason;
    return true;
  }
  else
    return false;
}

int STM_WDG_GetReason(void)
{
  _calcGapAddress();            // can be not initialized after RESET

  if ((_memGapAddress != NULL) && (*_memGapAddress == WDG_MAGIC_CONSTANT))
  {
    return _memGapAddress[1];
  }

  return -1;                    // fail as return value
}

uint32_t STM_WDG_GetMagic(void)
{
  _calcGapAddress();            // can be not initialized after RESET

  if (_memGapAddress != NULL)
    return _memGapAddress[0];
  else
    return 0;
}
