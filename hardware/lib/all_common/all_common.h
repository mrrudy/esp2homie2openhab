#ifndef all_common_H_
#define all_common_H_
#include "userconf.h"
#include <Homie.h>

void all_common_setup();
void all_common_loop();

#if defined(USERLIB_USE_WATCHDOG_WIFI) || defined(__DOALL__)

  #include <Ticker.h>
  void ICACHE_RAM_ATTR ISRwatchdog(void);

  extern Ticker watchdogTick;
  extern unsigned long watchdogLastFeed;     //ensure that the watchdog does not bite before setup

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#endif /*all_common_H_*/
