#ifndef _USERCONF_H_
#define _USERCONF_H_
/**
 * @brief   Enables the Watchdog WIFI subsystem.
 */

 #if !defined(BOARD_BUTTONS)
 #define BOARD_BUTTONS 0
 #endif

 #if !defined(BOARD_SHUTTERS)
 #define BOARD_SHUTTERS 0
 #endif

 #if !defined(BOARD_SWITCHES)
 #define BOARD_SWITCHES 0
 #endif

 #if !defined(USERLIB_USE_WATCHDOG_WIFI)
 #define USERLIB_USE_WATCHDOG_WIFI             true
 #endif

#if !defined(BOARD_FAMILY_NAME)
#define BOARD_FAMILY_NAME family_not_defined
#endif

#if !defined(BOARD_NAME)
#define BOARD_NAME name_not_defined
#endif

#if !defined(VERSION)
#define VERSION 0.00
#endif

#if USERLIB_USE_WATCHDOG_WIFI==true
  #if !defined(WATCHDOGTIMEOUT)
    #define WATCHDOGTIMEOUT       60*1000 //time after which the watchdog resets
  #endif
#endif

#if BOARD_FAMILY_NAME==inCan
#define LEFTRELAYPIN 5
#define RIGHTRELAYPIN 13
#define BUTTONPIN1 14
#define BUTTONPIN2 12
#define BUTTONPIN3 0
#define LEDPIN 15
#define LEFT HIGH       //left channel is NO and activated by HIGH
#define RIGHT LOW
#define LEDOFF HIGH
#define LEDON LOW

#endif /* inCan */

#endif /* _USERCONF_H_ */
