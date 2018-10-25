#include <Arduino.h>
#include "all_common.h"

#if defined(USERLIB_USE_WATCHDOG_WIFI) || defined(__DOALL__)
unsigned long watchdogLastFeed=999999;
Ticker watchdogTick;

void ICACHE_RAM_ATTR ISRwatchdog(void)
{
  Serial.printf("Watchdog check: %lu\n\r", millis());
  if (millis()-watchdogLastFeed > watchdogTimeout)
  {
    Serial.println("Watchdog resets.");
    Serial.printf("Current time: %lu, last feed: %lu, difference: %lu\n\r", millis(), watchdogLastFeed, millis()-watchdogLastFeed);
    ESP.restart();
  }
}
#endif /*USERLIB_USE_WATCHDOG_WIFI*/

void all_common_setup(){
#if defined(USERLIB_USE_WATCHDOG_WIFI) || defined(__DOALL__)
    watchdogLastFeed=millis()+5*1000; //give setup some additional time, enought to run its magic
    Serial.printf("initializing watchdog with margin: %lu, current time is: %lu\n\r", watchdogLastFeed, millis());
    watchdogTick.attach(10, ISRwatchdog);
}

void all_common_loop(){

  if(Homie.isConnected()) { watchdogLastFeed=millis(); }//if homie is connected feed the dog

}

#endif /*USERLIB_USE_WATCHDOG_WIFI*/
