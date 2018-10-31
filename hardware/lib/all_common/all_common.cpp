#include <Arduino.h>
#include "all_common.h"
#include "all_common_log.h"

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
        Homie.disableLogging();   //there is a bug and if this is enabled you get a boot loop
  #endif
        Homie.disableResetTrigger();    //if you want to use GPIO0 you need to disable this as it will reset your configuration when LOW

        Homie.setup();

#if defined(USERLIB_USE_WATCHDOG_WIFI)
        watchdogLastFeed=millis()+5*1000; //give setup some additional time, enought to run its magic
        Debugf("initializing watchdog with margin: %lu, current time is: %lu\n\r", watchdogLastFeed, millis());
        watchdogTick.attach(10, ISRwatchdog);
#endif /*USERLIB_USE_WATCHDOG_WIFI*/

        Debugf("\n\nValue of GPIO_BUTTONS: %d\n\n", BOARD_BUTTONS);

#if defined(BOARD_BUTTONS)

        for(int i=0; i<BOARD_BUTTONS; i++) {
                buttons[i].oneButtonInsance.attachClick(default_click_handler);
                buttons[i].oneButtonInsance.attachDoubleClick(default_doubleclick_handler);
                buttons[i].oneButtonInsance.attachLongPressStart(default_longclick_handler);

                buttons[i].buttonHomieNode.setProperty("datatype").send("enum");
                buttons[i].buttonHomieNode.setProperty("format").send("click,2click,longclick");
                buttons[i].buttonHomieNode.advertise("datatype");
                buttons[i].buttonHomieNode.advertise("format");
                buttons[i].buttonHomieNode.advertise("event");
                Debugf("Setting up button: %d", i);
        }
#endif
}

void all_common_loop(){
#if defined(USERLIB_USE_WATCHDOG_WIFI)
        if(Homie.isConnected()) { watchdogLastFeed=millis(); }//if homie is connected feed the dog

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#if defined(BOARD_BUTTONS)
        for(int i=0; i<BOARD_BUTTONS; i++) {
                buttons[i].oneButtonInsance.tick();
//  Serial.printf("Ticking for button: %d", i);
        }
#endif

}

#if defined(BOARD_BUTTONS)

void null_function(void) {
        Debug("No function on MC attached to me (i'm null_function())");
}

void default_click_handler(void) {
        for(int i=0; i<BOARD_BUTTONS; i++) {
                if(buttons[i].oneButtonInsance.wasIPressed()) {
                        Debugf("Button number: %d, got CLICK, my Ticks= %d", i, buttons[i].oneButtonInsance.getPressedTicks());
                        buttons[i].buttonHomieNode.setProperty("event").send("click");
                        buttons[i].click_handler();
                        break;
                }
        }
}

void default_doubleclick_handler(void) {
        for(int i=0; i<BOARD_BUTTONS; i++) {
                if(buttons[i].oneButtonInsance.wasIPressed()) {
                        Debugf("Button number: %d, got 2CLICK, my Ticks= %d", i, buttons[i].oneButtonInsance.getPressedTicks());
                        buttons[i].buttonHomieNode.setProperty("event").send("2click");
                        buttons[i].doubleclick_handler();
                        break;
                }
        }
}

void default_longclick_handler(void) {
        for(int i=0; i<BOARD_BUTTONS; i++) {
                if(buttons[i].oneButtonInsance.wasIPressed()) {
                        Debugf("Button number: %d, got LongCLICK, my Ticks= %d", i, buttons[i].oneButtonInsance.getPressedTicks());
                        buttons[i].buttonHomieNode.setProperty("event").send("longclick");
                        buttons[i].longclick_handler();
                        break;
                }
        }
}


#endif
