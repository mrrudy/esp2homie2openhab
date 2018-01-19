#include <Arduino.h>
#include <Homie.h>

void setup() {
//        Homie.disableLogging();
        Homie_setFirmware("1M64OTAplatf", "0.2");
//        Homie.setLedPin(13, HIGH);
        Homie.setup();

        // put your setup code here, to run once:
}

void loop() {
  Homie.loop();
        // put your main code here, to run repeatedly:
}
