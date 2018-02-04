#include <Arduino.h>
#include <Homie.h>
#include "main.h"
#include <Thermostat.h>
#include <DHT.h>

Thermostat thermostat;
DHT dht(DHTPIN, DHTTYPE);

HomieNode currentTempNode("currentTemp", "temperature");
HomieNode desiredTempNode("desiredTemp", "temperature");

HomieSetting<long> temperatureIntervalSetting("temperatureInterval", "The temperature interval in seconds ");
HomieSetting<double> histeresisSetting("histeresis", "The histeresis of the temp sensor ");


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

        float currentTemp=dht.readTemperature(false);
        if (isnan(currentTemp)) {
          Debug("Failed to read from DHT sensor!");
          return 0;
        }
        currentTempNode.setProperty("degrees").send(String(currentTemp));
        return currentTemp;
}

bool newDesiredTempHandler(const HomieRange& range, const String& value) {
        float temp=value.toFloat();
        if ( (temp || value == "0" || value == "0.0" || value == "0.00") && thermostat.setDesiredTemp(temp)) {
                ;
                desiredTempNode.setProperty("degrees").send(String(temp));
                return true;
        }
        return false;
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

        dht.begin();
        pinMode(RELAYPIN, OUTPUT);
        temperatureIntervalSetting.setDefaultValue(DEFAULT_TEMPERATURE_INTERVAL).setValidator([] (long candidate) {
                                                                                                      return candidate > 0;
                                                                                              });
        histeresisSetting.setDefaultValue(DEFAULT_HISTERESIS).setValidator([] (double candidate) {
                                                                                                      return candidate > 0;
                                                                                              });
        currentTempNode.advertise("unit");
        currentTempNode.advertise("datatype");
        currentTempNode.advertise("deegrees");

        desiredTempNode.advertise("unit");
        desiredTempNode.advertise("datatype");
        desiredTempNode.advertise("degrees").settable(newDesiredTempHandler);

        Homie.setup();

        thermostat
        .setup(temperatureIntervalSetting.get() * 1000UL, histeresisSetting.get())  //U can use homie settings only after Homie.setup()
        .setOperationHandler(serveThermostatOperationChange)
        .setReadoutHandler(performTempReadout)
        .setDesiredTemp(DEFAULT_DESIRED_TEMPERATURE)
        ;
        newDesiredTempHandler(HomieRange {true,0}, String("25")); // TODO:find a way to execute it instead of setting it separately in homie and thermostat; for now it does not have an effect
        Debug("I'm alive! After homie setup. ");

}

void loop() {

        Homie.loop();
        thermostat.loop();
        // put your main code here, to run repeatedly:
}
