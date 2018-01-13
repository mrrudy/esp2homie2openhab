#include <Arduino.h>
#include "relay.h"


bool relaySwitch(int whichRelay, int channel) {
        digitalWrite(whichRelay, channel);
        return (true);
}
