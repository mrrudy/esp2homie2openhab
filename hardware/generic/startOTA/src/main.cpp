#include <Arduino.h>
#include <Homie.h>
#include "main.h"

void setup() {
//        Homie.disableLogging();
        Homie_setFirmware(BOARD_NAME, VERSION);
//        Homie.setLedPin(13, HIGH);
        Homie.setup();

        // put your setup code here, to run once:
}

void loop() {
  Homie.loop();
        // put your main code here, to run repeatedly:
}
