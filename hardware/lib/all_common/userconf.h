#ifndef _USERCONF_H_
#define _USERCONF_H_
/**
 * @brief   Enables the Watchdog WIFI subsystem.
 */
#if !defined(USERLIB_USE_WATCHDOG_WIFI) || defined(__DOALL__)
#define USERLIB_USE_WATCHDOG_WIFI             FALSE
#endif

#if defined(USERLIB_USE_WATCHDOG_WIFI) || defined(__DOALL__)
  #if !defined(watchdogTimeout)
    #define watchdogTimeout       60*1000 //time after which the watchdog resets
  #endif
#endif






#endif /* _USERCONF_H_ */
