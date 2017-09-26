#include <Homie.h>

void setup() {
Homie.disableLogging();
  

Homie.disableLedFeedback();
  Homie_setFirmware("bare-minimum", "1.0.7"); // The underscore is not a typo! See Magic bytes
  Homie.setup();
  
//Serial.begin(115200);
}

void loop() {
  Homie.loop();
}
