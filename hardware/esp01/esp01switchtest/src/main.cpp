
#include "Arduino.h"
#include "main.h"
#include <Homie.h>


int lastReadoutTime=0;
bool swap_high=true;

//HomieNode relay1("center", "switch");

bool relayOnHandler(const HomieRange& range, const String& value) {
    relaySwitch(RELAYPIN,value=="true" ? HIGH : LOW);
//    digitalWrite(0, value=="true" ? HIGH : LOW);
    Debugf("Switching relay to %d", value=="true" ? HIGH : LOW);
//    relay1.setProperty("on").send(value); //
    return true;
}



void setup() {
//          Homie.disableLogging();
Serial.begin(74880); //if you want logging on serial comment above line so it is not disabled
Homie.disableResetTrigger();  //if you want to use GPIO0 you need to disable this as it will reset your configuration when LOW
          Homie_setFirmware(BOARD_NAME, VERSION);
          //Homie.setLedPin(LEDPIN, LOW);
          Homie.disableLedFeedback();

//          relay1.advertise("on").settable(relayOnHandler);

//          pinMode(0, FUNCTION_3);
          pinMode(RELAYPIN, OUTPUT);
          digitalWrite(RELAYPIN, HIGH);
//          pinMode(2, OUTPUT);

          Homie.setup();
          Homie.getMqttClient().setKeepAlive(300);
}

void loop() {
  Homie.loop();
  if (millis()-lastReadoutTime>=1500) {
  lastReadoutTime=millis();
Debugf("wolny heap: %d", ESP.getFreeHeap());
//  digitalWrite(0, swap_high ? HIGH : LOW);
//  swap_high=!swap_high;
  }

}
