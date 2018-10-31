#ifndef _USERCONF_H_
#define _USERCONF_H_
/**
 * @brief   Enables the Watchdog WIFI subsystem.
 */

#if !defined(BOARD_BUTTONS)
#define BOARD_BUTTONS 0
#endif

#if !defined(USERLIB_USE_WATCHDOG_WIFI)
  #define USERLIB_USE_WATCHDOG_WIFI             false
#endif

#if defined(USERLIB_USE_WATCHDOG_WIFI)
  #if !defined(WATCHDOGTIMEOUT)
    #define WATCHDOGTIMEOUT       60*1000 //time after which the watchdog resets
  #endif
#endif

#endif /* _USERCONF_H_ */
