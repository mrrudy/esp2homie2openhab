#include <Homie.h>
#include <OneButton.h>
#include <EEPROM.h>
#include <Shutters.h>

#include "main.h"

#if defined(BOARD_BUTTONS)
char default_prefix[]="button_";
GPIO_button buttons[BOARD_BUTTONS] = {
                                              {BUTTONPIN1, (char *)"1", default_prefix, null_function, null_function, null_function, OneButton (BUTTONPIN1, true), HomieNode ("but1", "button")}
                                              ,{BUTTONPIN2, (char *)"2", default_prefix, click2, null_function, null_function, OneButton (BUTTONPIN2, true), HomieNode ("but2", "button")}
                                              ,{BUTTONPIN3, (char *)"3", default_prefix, click1, null_function, null_function, OneButton (BUTTONPIN3, true), HomieNode ("but3", "button")}
                                            };
#endif

const byte eepromOffset = 0;
//const unsigned long upCourseTime = 21 * 1000;
//const unsigned long downCourseTime = 21 * 1000;
const float calibrationRatio = 0.2;
//TODO: add GPIObuttonForHomie(PIN, "name_preffix")


HomieNode blinds("blinds1", "blinds");

Shutters shutters; //

byte whichRelayIsWorking = UPRELAY;

HomieSetting<long> shuttersCourseTimeUP("courseTimeUP", "How long (ms) does it take for shutters to run UP");
HomieSetting<long> shuttersCourseTimeDOWN("courseTimeDOWN", "How long (ms) does it take for shutters to run DOWN");
HomieSetting<bool> shuttersReverseRelays("reverseRelays", "Should UP and DOWN relays be switched?");

void shuttersOperationHandler(Shutters* s, ShuttersOperation operation) {
//        relaySwitch(UPRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
//        relaySwitch(DOWNRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
        switch (operation) {
        case ShuttersOperation::UP:
                whichRelayIsWorking=(shuttersReverseRelays.get()) ? DOWNRELAY : UPRELAY;
                Debugf("Shutters going up. Switching relay: %d", whichRelayIsWorking);
                relaySwitch(whichRelayIsWorking,ENGINELINE); //switch the motor relay to the line with up shutters
                break;
        case ShuttersOperation::DOWN:
                whichRelayIsWorking=(shuttersReverseRelays.get()) ? UPRELAY : DOWNRELAY;
                Debugf("Shutters going down. Switching relay: %d", whichRelayIsWorking);
                relaySwitch(whichRelayIsWorking,ENGINELINE); //switch the motor relay to the line with up shutters
                break;
        case ShuttersOperation::HALT:
                Debug("Shutters stop.");
                relaySwitch(whichRelayIsWorking,!ENGINELINE); //whatever we will do first we stop the shutters from moving
//                relaySwitch(DOWNRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
                break;
        }
}

void readInEeprom(char* dest, byte length) {
        for (byte i = 0; i < length; i++) {
                dest[i] = EEPROM.read(eepromOffset + i);
        }
        Debugf("read from eeprom position: %d %%", (int) dest);
}

void shuttersWriteStateHandler(Shutters* shutters, const char* state, byte length) {
        for (byte i = 0; i < length; i++) {
                EEPROM.write(eepromOffset + i, state[i]);
    #ifdef ESP8266
                EEPROM.commit();
    #endif
        }
}

void onShuttersLevelReached(Shutters* shutters, byte level) {
        if(level%10==0) {
//        Debugf("shutters reached level: %d %%", level);
                blinds.setProperty("position").send(String(level)); //send the position
        }
}

bool blindsMoveHandler(const HomieRange& range, const String& value) {
        int new_position_in_percent=0;

        if(!shutters.isIdle()) { //if we received new position and if blinds are moving stop them first
                shutters.stop();
        }
        if(value=="STOP") return(true);
        if((value=="UP")||(value=="DOWN")) {
                new_position_in_percent=(value=="UP") ? 0 : 100;
        }else new_position_in_percent=value.toInt();

        Debugf("received new blinds position request: %d %%", new_position_in_percent);
        shutters.setLevel(new_position_in_percent);
}



// This function will be called when the button1 was pressed shortly.
void click1() {
        if(!shutters.isIdle()) { //if blinds are moving stop them
                shutters.stop();
                Debug("stopping shutters becouse of click1");
        }
        else {
                shutters.setLevel((int)100); //move them 100% (down)
                Debug("moving shutters down becouse of click1");
        }

} // click1

// This function will be called when the button2 was pressed shortly.
void click2() {
        if(!shutters.isIdle()) { //if blinds are moving stop them
                shutters.stop();
                Debug("stopping shutters becouse of click2");
        }
        else {
                shutters.setLevel((int)0); //move them 0% (up)
                Debug("moving shutters up becouse of click2");
        }
}


void setup() {

        pinMode(UPRELAY, OUTPUT);
        pinMode(DOWNRELAY, OUTPUT);

        relaySwitch(UPRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
        relaySwitch(DOWNRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving




//########## HOMIE stuff
#ifdef DEBUG_ENABLED
        Serial.begin(74880);
#else
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
#endif
        Homie.disableResetTrigger();  //if you want to use GPIO0 you need to disable this as it will reset your configuration when LOW
        Homie.setLedPin(LEDPIN, HIGH);
        Homie_setFirmware(BOARD_FAMILY_NAME "-" BOARD_NAME "-" BOARD_FUTURES, VERSION);

        shuttersCourseTimeUP.setDefaultValue(21000).setValidator([] (long candidate) {
                return candidate >= 1000;
        });
        shuttersCourseTimeDOWN.setDefaultValue(0).setValidator([] (long candidate) {
                return candidate >= 0;
        });
        shuttersReverseRelays.setDefaultValue(false).setValidator([] (bool candidate) {
                return (candidate == true) || (candidate == false);
        });

        Homie.setup();

        blinds.setProperty("position").send("NaN");
        blinds.setProperty("unit").send("%");
        blinds.setProperty("format").send("integer");
        blinds.advertise("position").settable(blindsMoveHandler);
        blinds.advertise("unit");
        blinds.advertise("format");

  #ifdef ESP8266
        EEPROM.begin(512);
  #endif
        char storedShuttersState[shutters.getStateLength()];
        readInEeprom(storedShuttersState, shutters.getStateLength());
        shutters
        .setOperationHandler(shuttersOperationHandler)
        .setWriteStateHandler(shuttersWriteStateHandler)
        .restoreState(storedShuttersState)
        .setCourseTime(shuttersCourseTimeUP.get(), shuttersCourseTimeDOWN.get())
        .onLevelReached(onShuttersLevelReached)
        .begin()
//        .setLevel(30) // Go to 30%
        ;
        Debugf("\nGPIO_BUTTONS in setup: %d\n", BOARD_BUTTONS);
        all_common_setup();
}

void loop() {
        all_common_loop();
        Homie.loop();
        shutters.loop();
}
