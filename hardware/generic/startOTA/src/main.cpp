#include <Arduino.h>
#include <Homie.h>
#include "main.h"

//char _all_common_log_debugbuffer[150];

void setup() {
//        Homie.disableLogging();
        Homie_setFirmware(BOARD_NAME, VERSION);
//        Homie.setLedPin(13, HIGH);
        Homie.setup();
        Debugf("I'm alive! OTA me pls.");
        // put your setup code here, to run once:
}

void loop() {
        Homie.loop();
        // put your main code here, to run repeatedly:
}
