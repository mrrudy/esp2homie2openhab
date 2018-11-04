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

typedef void (button_handler)(void);
void null_function(void);

class BoardButton {
private:
  BoardButton& PropagateHomieStatus(const String& value);
  static button_handler DefaultHandlerAction;

public:
  BoardButton(int GPIO, const char *name, button_handler click_handler=DefaultHandlerAction, button_handler doubleclick_handler=DefaultHandlerAction, button_handler longclick_handler=DefaultHandlerAction);
  int _GPIO;
  const char *_name;
  button_handler *_click_handler;
  button_handler *_doubleclick_handler;
  button_handler *_longclick_handler;
  HomieNode _HomieNode;
  OneButton _OneButton;
};

extern BoardButton buttons[];

#endif /*BOARD_BUTTONS*/

#if BOARD_SWITCHES > 0
bool defaultRelayHandler(const HomieRange& range, const String& value);

//typedef bool (RelayHandler)(const HomieRange& range, const String& value);

class BoardSwitch {
private:

public:
  int _GPIO;
  const char *_name;
  HomieNode _HomieNode;
  BoardSwitch();
  BoardSwitch(int GPIO, const char *name);
};


extern BoardSwitch switches[];

#endif /*BOARD_SWITCHES*/


#endif /*all_common_H_*/
