#include <Homie.h>
#include "OneButton.h"

#define BOARD_NAME suplav3
#define DEBUG_PREFIX "DEBUG/suplav3"
#define DEBUG_ENABLED 1
#define RELAYPIN1 5
#define RELAYPIN2 13

OneButton button1(14, true);
OneButton button2(12, true);

/*
#include "DHT.h"
#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define TEMP_READ_FREQ 5000 //read temperature every 5 sec
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTemperatureSent = 0;
*/

int switch_state; //holds the state in which the switch should be

HomieNode relay1("1", "switch");
HomieNode relay2("2", "switch");
//HomieNode temperatureNode("temperature", "temperature");



bool relay1OnHandler(const HomieRange& range, const String& value) {
  int mask = RELAYPIN1;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay1.setProperty("on").send(value); // TODO: change to self
    return true;
  }
  else return false;
}

bool relay2OnHandler(const HomieRange& range, const String& value) {
  int mask = RELAYPIN2;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay2.setProperty("on").send(value); // TODO: change to self
    return true;
  }
  else return false;
}

bool relaySwitch(int mask, const String& value) {
  #if DEBUG_ENABLED
   char buffer[50];
  #endif
  if (value != "true" && value != "false") return false;
  bool on = (value == "true");
  if(on) {
    digitalWrite(mask, HIGH); //turns on relay
  }
  else {
    digitalWrite(mask, LOW);
  }
  #if DEBUG_ENABLED
  sprintf(buffer, "changing switch no. %d to state: %s", mask, on ? "ON" : "OFF");
  Homie.getMqttClient().publish("DEBUG/BOARD_NAME", 1, false, buffer);
  #endif
  return (true);
}

void buttonLoop() {
#if DEBUG_ENABLED
 char buffer[50];
#endif
                    #if DEBUG_ENABLED
                    sprintf(buffer, "received on serial: %#08x", 111);
                    Homie.getMqttClient().publish("DEBUG/BOARD_NAME", 1, false, buffer);
                    #endif

}

void setup() {
  pinMode(RELAYPIN1, OUTPUT);
  pinMode(RELAYPIN2, OUTPUT);

  button1.attachClick(click1);
  button1.attachDoubleClick(doubleclick1);
  button1.attachLongPressStart(longPressStart1);
//  dht.begin();

//########## HOMIE stuff
  Homie.disableLogging();
//  Homie.setLedPin(15, HIGH);
  Homie_setFirmware("BOARD_NAME_switch_OTA", "1.0.3");
  Homie.setup();
  
  relay1.advertise("on").settable(relay1OnHandler);
  relay1OnHandler({true,0}, "false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch
  relay2.advertise("on").settable(relay2OnHandler);
  relay2OnHandler({true,0}, "false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch
/*  
  temperatureNode.setProperty("unit").send("c");
  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");
*/
//  Serial.begin(19200);
}

void loop() {
unsigned long now = millis();
  Homie.loop();
  button1.tick();
}

void click1() {
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX, 1, false, "button1 click");
  #endif
} // click1


// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish("DEBUG/BOARD_NAME", 1, false, "button1 doubleclick");
  #endif
} // doubleclick1


// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStart1() {
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish("DEBUG/BOARD_NAME", 1, false, "button1 longpress stop");
  #endif
} // longPressStop1

