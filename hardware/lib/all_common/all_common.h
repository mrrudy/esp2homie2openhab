#ifndef all_common_H_
#define all_common_H_
#include "userconf.h"
#include <Homie.h>

void all_common_setup();
void all_common_loop();

#if defined(USERLIB_USE_WATCHDOG_WIFI)

  #include <Ticker.h>
  void ICACHE_RAM_ATTR ISRwatchdog(void);

  extern Ticker watchdogTick;
  extern unsigned long watchdogLastFeed;     //ensure that the watchdog does not bite before setup

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#if BOARD_BUTTONS > 0

#include <OneButton.h>

struct GPIO_button {
  int GPIO;
  const char *name;
  void (*click_handler)(void);
  void (*doubleclick_handler)(void);
  void (*longclick_handler)(void);
  HomieNode *HomieNodeptr;
  OneButton *oneButtonptr;
};

typedef struct GPIO_button GPIO_button;
extern GPIO_button buttons[];

void null_function(void);

void default_click_handler(void);
void default_doubleclick_handler(void);
void default_longclick_handler(void);

#endif /*BOARD_BUTTONS*/

#if BOARD_SWITCHES > 0
struct GPIO_switch {
  int GPIO;
  const char *name;
  bool (*relay_handler)(const HomieRange& range, const String& value);
  HomieNode *HomieNodeptr;
};

typedef struct GPIO_switch GPIO_switch;
extern GPIO_switch switches[];
bool defaultRelayHandler(const HomieRange& range, const String& value);

#endif /*BOARD_SWITCHES*/


#endif /*all_common_H_*/
