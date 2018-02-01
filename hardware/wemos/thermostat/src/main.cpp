#include <Arduino.h>
#include <Homie.h>
#include "main.h"
#include <Thermostat.h>
#include <DHT.h>

Thermostat thermostat;
DHT dht;

HomieNode currentTempNode("currentTemp", "temperature");
HomieNode desiredTempNode("desiredTemp", "temperature");

HomieSetting<long> temperatureIntervalSetting("temperatureInterval", "The temperature interval in seconds ");


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
        float currentTemp=dht.getTemperature();
        currentTempNode.setProperty("degrees").send(String(currentTemp));
        return currentTemp;
}

bool newDesiredTempHandler(const HomieRange& range, const String& value) {
        desiredTempNode.setProperty("degrees").send(value);

        return thermostat.setDesiredTemp(value.toFloat());
        ;
}

void setupHandler() {
        currentTempNode.setProperty("unit").send("c");
        currentTempNode.setProperty("datatype").send("float");

        desiredTempNode.setProperty("unit").send("c");
        desiredTempNode.setProperty("settable").send("true");
        desiredTempNode.setProperty("datatype").send("float");

        desiredTempNode.setProperty("degrees").send(String(DEFAULT_DESIRED_TEMPERATURE));
}
void setup() {
//        Homie.disableLogging();
        Homie_setFirmware(BOARD_NAME, VERSION);
        Homie.setSetupFunction(setupHandler);
//        Homie.setLedPin(13, HIGH);
//        Homie.setup();
        // put your setup code here, to run once:
        dht.setup(DHTPIN);
        pinMode(RELAYPIN, OUTPUT);
        temperatureIntervalSetting.setDefaultValue(DEFAULT_TEMPERATURE_INTERVAL).setValidator([] (long candidate) {
                                                                                                      return candidate > 0;
                                                                                              });
        thermostat
        .setup(temperatureIntervalSetting.get() * 1000UL, 0.5)
        .setOperationHandler(serveThermostatOperationChange)
        .setReadoutHandler(performTempReadout)
        .setDesiredTemp(DEFAULT_DESIRED_TEMPERATURE)
        ;

        currentTempNode.advertise("unit");
        currentTempNode.advertise("datatype");
        currentTempNode.advertise("deegrees");

        desiredTempNode.advertise("unit");
        desiredTempNode.advertise("datatype");
        desiredTempNode.advertise("degrees").settable(newDesiredTempHandler);


        Homie.setup();
        //desiredTempNode.setProperty("degrees").send(String(DEFAULT_DESIRED_TEMPERATURE));
}

void loop() {

        Homie.loop();
        thermostat.loop();
        // put your main code here, to run repeatedly:
}
