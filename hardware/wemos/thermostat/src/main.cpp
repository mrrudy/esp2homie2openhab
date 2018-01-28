#include <Arduino.h>
#include <Homie.h>
#include "main.h"
#include <Thermostat.h>
#include <DHT.h>

Thermostat thermostat;
DHT dht;

HomieNode currentTempNode("currentTemp", "temperature");
HomieNode desiredTempNode("desiredTemp", "temperature");

void serveThermostatOperationChange(Thermostat *t, ThermostatOperation operation) {
        switch (operation) {
        case ThermostatOperation::HEATING_ON:
                digitalWrite(RELAYPIN, RELAY_NC_OFF);
                break;
        case ThermostatOperation::HEATING_OFF:
                digitalWrite(RELAYPIN, !RELAY_NC_OFF);
                break;
        }
}

float performTempReadout(Thermostat *t) {
        return dht.getTemperature();
}

bool newDesiredTempHandler(const HomieRange& range, const String& value) {
        return thermostat.setDesiredTemp(value.toFloat());
        ;
}
void setup() {
//        Homie.disableLogging();
        Homie_setFirmware(BOARD_NAME, VERSION);
//        Homie.setLedPin(13, HIGH);
        Homie.setup();
        // put your setup code here, to run once:
        dht.setup(DHTPIN);
        pinMode(RELAYPIN, OUTPUT);
        thermostat
        .setup(5*1000, 0.5)
        .setOperationHandler(serveThermostatOperationChange)
        .setReadoutHandler(performTempReadout)
        .setDesiredTemp(21)
        ;
        currentTempNode.setProperty("unit").send("c");
        currentTempNode.setProperty("settable").send("false");
        currentTempNode.setProperty("datatype").send("float");
        currentTempNode.advertise("datatype");
        currentTempNode.advertise("unit");
        currentTempNode.advertise("degrees");

        desiredTempNode.setProperty("unit").send("c");
        desiredTempNode.setProperty("settable").send("true");
        desiredTempNode.setProperty("datatype").send("float");
        desiredTempNode.advertise("unit");
        desiredTempNode.advertise("datatype");
        desiredTempNode.advertise("degrees").settable(newDesiredTempHandler);
}

void loop() {
        Homie.loop();
        thermostat.loop();
        // put your main code here, to run repeatedly:
}
