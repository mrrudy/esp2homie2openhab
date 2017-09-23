#include <Homie.h>

#define DEBUG_ENABLED 1
#define RELAYPIN D1

#include "DHT.h"
#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define TEMP_READ_FREQ 5000 //read temperature every 5 sec

DHT dht(DHTPIN, DHTTYPE);

long lastMsg = 0;

HomieNode relay1("1", "switch");
int switch_state; //holds the state in which the switch should be

bool relay1OnHandler(const HomieRange& range, const String& value) {
  int mask = B01;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay1.setProperty("on").send(value); // TODO: change to self
    return true;
  }
  else return false;
}

bool relaySwitch(int mask, const String& value) {
  if (value != "true" && value != "false") return false;
  bool on = (value == "true");
  if(on) {
    digitalWrite(RELAYPIN, HIGH); //turns on relay
  }
  else {
    digitalWrite(RELAYPIN, LOW);
  }
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish("DEBUG/sonoff", 1, false, "changing switch state");
  #endif
  return (true);
}

void buttonLoop() {
#if DEBUG_ENABLED
 char buffer[50];
#endif
                    #if DEBUG_ENABLED
                    sprintf(buffer, "received on serial: %#08x", 111);
                    Homie.getMqttClient().publish("DEBUG/sonoff", 1, false, buffer);
                    #endif

}

void setup() {
  pinMode(RELAYPIN, OUTPUT);
  dht.begin();
//########## HOMIE stuff
//  Homie.disableLogging();
//  Homie.setLedPin(13, HIGH);
  Homie_setFirmware("wemosD1_DHT_switch_OTA", "1.0.0");
  Homie.setup();
  relay1.advertise("on").settable(relay1OnHandler);
  relay1OnHandler({true,0}, "false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch
//  Serial.begin(19200);
}

void loop() {
long now = millis();
Homie.loop();
  if (now - lastMsg > TEMP_READ_FREQ) {
    lastMsg = now;
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) ) {
                    #if DEBUG_ENABLED
                    Homie.getMqttClient().publish("DEBUG/sonoff", 1, false, "Failed to read from DHT sensor!");
                    #endif

      }

  }
//  buttonLoop();
}
