#include <Arduino.h>
#include "all_common.h"
#include "all_common_log.h"
#include "all_common_relay.h"

#if defined(USERLIB_USE_WATCHDOG_WIFI)
unsigned long watchdogLastFeed=999999;
Ticker watchdogTick;

void ICACHE_RAM_ATTR ISRwatchdog(void)
{
        if (millis()-watchdogLastFeed > WATCHDOGTIMEOUT)
        {
                Debug("Watchdog resets.");
                ESP.restart();
        }
}
#endif /*USERLIB_USE_WATCHDOG_WIFI*/


void all_common_setup(){


#ifdef DEBUG_ENABLED
        Serial.begin(74880);
#else
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
#endif
        Homie.disableResetTrigger();  //if you want to use GPIO0 you need to disable this as it will reset your configuration when LOW

#if defined(USERLIB_USE_WATCHDOG_WIFI)
        watchdogLastFeed=millis()+5*1000; //give setup some additional time, enought to run its magic
        Debugf("\n\rinitializing watchdog with margin: %lu, current time is: %lu\n\r", watchdogLastFeed, millis());
        watchdogTick.attach(10, ISRwatchdog);
#endif /*USERLIB_USE_WATCHDOG_WIFI*/

        Homie.setup(); //needs to be after node declaraction if it is dynamic (new)
}

void all_common_loop(){
#if defined(USERLIB_USE_WATCHDOG_WIFI)
        if(Homie.isConnected()) { watchdogLastFeed=millis(); }//if homie is connected feed the dog

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#if BOARD_BUTTONS > 0
        for(int i=0; i<BOARD_BUTTONS; i++) {
                buttons[i]._OneButton.tick();
//  Serial.printf("Ticking for button: %d", i);
        }
#endif

}

#if BOARD_SWITCHES > 0

BoardSwitch::BoardSwitch(int GPIO, const char *name)
        :  _GPIO(GPIO)
        , _name(name)
        , _HomieNode(HomieNode(this->_name, "switch"))
{
  _HomieNode.advertise("on").settable(
          [this](const HomieRange& range, const String& value) -> bool {
            relaySwitch(this->_GPIO, value=="true" ? HIGH : LOW);
            this->_HomieNode.setProperty("on").send(value); return true;
          }
  );
}

#endif

#if BOARD_BUTTONS > 0

BoardButton& BoardButton::PropagateHomieStatus(const String& value){
      this->_HomieNode.setProperty("event").send(value);
      return *this;
};
void BoardButton::DefaultHandlerAction(void){
  Debug("No function on MC attached to me (i'm DefaultHandlerAction())");
};

BoardButton::BoardButton(int GPIO, const char *name, button_handler click_handler, button_handler doubleclick_handler, button_handler longclick_handler)
        : _GPIO(GPIO)
        , _name(name)
        , _click_handler(click_handler)
        , _doubleclick_handler(doubleclick_handler)
        , _longclick_handler(longclick_handler)
        , _HomieNode(HomieNode(this->_name, "button"))
        , _OneButton(OneButton(this->_GPIO,true))
{

        _HomieNode.setProperty("datatype").send("enum");
        _HomieNode.setProperty("format").send("click,2click,longclick");
        _HomieNode.advertise("datatype");
        _HomieNode.advertise("format");
        _HomieNode.advertise("event");
        _OneButton.attachClick(
                (
                        [this](void) {
                          this->PropagateHomieStatus("click");
                          this->_click_handler();
                          return;
                        }
                )
        );
        _OneButton.attachDoubleClick(
                (
                        [this](void) {
                          this->PropagateHomieStatus("2click");
                          this->_doubleclick_handler();
                          return;
                        }
                )
        );
        _OneButton.attachLongPressStart(
                (
                        [this](void) {
                          this->PropagateHomieStatus("longclick");
                          this->_longclick_handler();
                          return;
                        }
                )
        );
}


#endif
