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

For WDG reason detection it requires modification of .LD file to create 2x 32bit gap on the top of RAM ("behind" the STACK):
```C++
...
/* _estack = ORIGIN(RAM) + LENGTH(RAM); */ /* end of "RAM" Ram type memory */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8; /* end of "RAM" Ram type memory minus 8B gap */
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

