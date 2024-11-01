# kei_pmk_watchdog
Simple files (library) for xWDG - for KEI/PMK

Usable functions (see header file):
```C++
bool STM_IWDG_Init(int timeMs);
bool STM_IWDG_Reset(void);
bool STM_IWDG_SetReason(int reason);
int STM_WDG_GetReason(void);
uint32_t STM_WDG_GetMagic(void);
```

For WDG reason detection it requires modification of .LD file to create 2x 32bit gap on the top of RAM ("behind" the STACK). Need to add new MEMORY area WDGRAM, modify size of RAM area and add new segment/section:
```C++
...
MEMORY
{
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 128k - 8  /* 128 * 1024 - 8 */
  WDGRAM (arw) : ORIGIN = 0x20001ff8, LENGTH = 8
  FLASH   (rx) : ORIGIN =  0x8000000, LENGTH = 512K
}
...
  .wdg_ram_data :
  {
    . = ALIGN(4);
    _swdg_ram_data = .;
    *(.wdg_ram_data);
    _ewdg_ram_data = .;
  } >WDGRAM
...
```
***
Using of lib. functions (simle example):
```C++
...
int main(void)
{
  ...
  ... // start LSI
  ...
  STM_IWDG_Init(1000);                // 1 sec = 1000 msec
  ...
  while(1)
  {
    ... // every XX milisec 
    {
      STM_IWDG_Reset();               // make WDG restart
    }

    {
      ... // some critical functionality which can "hangs" execution
      STM_IWDG_SetReason(WDG_CRITICAL_CALCULATE);   // set for detection after WDG fire

      ... // do it for long time (maybe)
    }
  }
}
```
***
Structure for storing last WDG action and text description
```C++
typedef struct _intCharPtrPairStruct
{
    int key;
    const char *text;
} IntCharPtrPair;
```
... and using like this definition:
```C++
const IntCharPtrPair _wdgDescriptions[] =
{
  ...
  { -1, NULL },         // detection of last element
};
```

