#include <Arduino.h>
#include "all_common_relay.h"


bool relaySwitch(int whichRelay, int channel) {
        digitalWrite(whichRelay, channel);
        return (true);
}
