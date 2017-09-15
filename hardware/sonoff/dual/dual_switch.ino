#include <Homie.h>

#define DEBUG_ENABLED 1

HomieNode relay1("left", "switch");
HomieNode relay2("right", "switch");
int switch_state; //holds the state in which the switch should be

bool relay1OnHandler(const HomieRange& range, const String& value) {
  int mask = B01;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay1.setProperty("on").send(value); // TODO: change to self
    return true;
  }
  else return false;
}

bool relay2OnHandler(const HomieRange& range, const String& value) {
  int mask = B10;  //relay number 2 (right)
  if(relaySwitch(mask, value)) {
    relay2.setProperty("on").send(value); // TODO: change to self
    return true;
  }
  else return false;
}

bool relaySwitch(int mask, const String& value) {
  if (value != "true" && value != "false") return false;
  bool on = (value == "true");
  if(on) {
    switch_state |= mask; //turns on relay
  }
  else {
    mask = ~mask; //inverts as this is turning off
    switch_state &= mask; //turns off relay TODO: this should be written only after the switch is set
  }
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish("DEBUG/sonoff", 1, false, "changing switch state");
  #endif
  return setSwitch(switch_state);
}

bool setSwitch(int newState) {
    Serial.write(0xA0);
    Serial.write(0x04);
    Serial.write(newState); //sends calculated switch state
    Serial.write(0xA1);
    Serial.flush();
  return true; //TODO: check the switch status if switched accordingly before confirming it is ok
}

void buttonLoop() {
#if DEBUG_ENABLED
 char buffer[50];
#endif
    if (Serial.available() >= 4) {
        unsigned char value;
        if (Serial.read() == 0xA0) {
            if (Serial.read() == 0x04) {
                value = Serial.read();
                if (Serial.read() == 0xA1) {
 
                    // RELAYs and BUTTONs are synchonized in the SIL F330
                    // The on-board BUTTON2 should toggle RELAY0 value
                    // Since we are not passing back RELAY2 value
                    // (in the relayStatus method) it will only be present
                    // here if it has actually been pressed
                    #if DEBUG_ENABLED
                    sprintf(buffer, "received on serial: %#08x", value);
                    Homie.getMqttClient().publish("DEBUG/sonoff", 1, false, buffer);
                    #endif
                    if ((value & 4) == 4) { 
                      value = value ^ 1;
                      relay1OnHandler({true,0}, "false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch
                      relay2OnHandler({true,0}, "false");
                    }

                    // Otherwise check if any of the other two BUTTONs
                    // (in the header) has been pressent, but we should
                    // ensure that we only toggle one of them to avoid
                    // the synchronization going mad
                    // This loop is generic for any PSB-04 module
/*                    for (unsigned int i=0; i<relayCount(); i++) {
 
                        bool status = (value & (1 << i)) > 0;
 
                        // relayStatus returns true if the status has changed
                        if (relayStatus(i, status)) break;
 
                    }*/
 
                }
            }
        }
 
    }
 
}



void setup() {
  Homie.disableLogging();
  Homie_setFirmware("sonoff_dual_switch_OTA", "1.0.6");
  relay1.advertise("on").settable(relay1OnHandler);
  relay2.advertise("on").settable(relay2OnHandler);
  Homie.setLedPin(13, HIGH);
  Homie.setup();
  Serial.begin(19200);
  relay1OnHandler({true,0}, "false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch
  relay2OnHandler({true,0}, "false");
}

void loop() {
  Homie.loop();
  buttonLoop();
}
