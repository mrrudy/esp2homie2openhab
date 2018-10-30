#ifndef all_common_H_
#define all_common_H_
#include "userconf.h"
#include <OneButton.h>
#include <Homie.h>

void all_common_setup();
void all_common_loop();

#if defined(USERLIB_USE_WATCHDOG_WIFI)

  #include <Ticker.h>
  void ICACHE_RAM_ATTR ISRwatchdog(void);

  extern Ticker watchdogTick;
  extern unsigned long watchdogLastFeed;     //ensure that the watchdog does not bite before setup

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#if defined (BOARD_BUTTONS)

void null_function(void);
void homie_button_handler(void);
void testowa(int x);

struct GPIO_button {
  int GPIO;
  char *name;
  char *prefix;
  void (*click_handler)(void);
  void (*doubleclick_handler)(void);
  void (*longclick_handler)(void);
  OneButton oneButtonInsance;
  HomieNode buttonHomieNode;
};

typedef struct GPIO_button GPIO_button;

//extern button buttons[];
extern GPIO_button buttons[];

void default_click_handler(void);
void default_doubleclick_handler(void);
void default_longclick_handler(void);

#endif /*GPIO_BUTTONS*/


#endif /*all_common_H_*/
