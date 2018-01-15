#include <Homie.h>
#include <EEPROM.h>
#include <Shutters.h>

#include <SonoffDual.h>

#include "main.h"

const byte eepromOffset = 0;
const unsigned long upCourseTime = 21 * 1000;
const unsigned long downCourseTime = 21 * 1000;
const float calibrationRatio = 0.2;

HomieNode button1Node("b1", "button");
HomieNode button2Node("b2", "button");
HomieNode button3Node("b3", "button");

HomieNode blinds("blinds1", "blinds");

Shutters shutters; //

void shuttersOperationHandler(Shutters* s, ShuttersOperation operation) {
  SonoffDual.setRelays(false, false);
        switch (operation) {
        case ShuttersOperation::UP:
                Debug("Shutters going up.");
                SonoffDual.setRelays(IS_UP_RELAY_LEFT,!IS_UP_RELAY_LEFT); //always switch only one of the relay on
                break;
        case ShuttersOperation::DOWN:
                Debug("Shutters going down.");
                SonoffDual.setRelays(!IS_UP_RELAY_LEFT,IS_UP_RELAY_LEFT); //always switch only one of the relay on
                break;
        case ShuttersOperation::HALT:
                Debug("Shutters stop.");
                SonoffDual.setRelays(false, false);
                break;
        }
}

void readInEeprom(char* dest, byte length) {
  dest[0]=0; return;
        for (byte i = 0; i < length; i++) {
                dest[i] = EEPROM.read(eepromOffset + i);
        }
        Debugf("read from eeprom position: %d %%", dest);
}

void shuttersWriteStateHandler(Shutters* shutters, const char* state, byte length) {
return;
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
        Debug("button1 click");
        button1Node.setProperty("event").send("click"); //sending this but be sure not to double react on it as below code already enables relay
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
        Debug("button2 click");
        button2Node.setProperty("event").send("click"); //sending this but be sure not to double react on it as below code already enables relay
        if(!shutters.isIdle()) { //if blinds are moving stop them
                shutters.stop();
                Debug("stopping shutters becouse of click2");
        }
        else {
                shutters.setLevel((int)0); //move them 0% (up)
                Debug("moving shutters up becouse of click2");
        }
}

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
        button1Node.setProperty("event").send("2click");
} // doubleclick1


// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStart1() {
        button1Node.setProperty("event").send("longclick");
} // longPressStop1


// This function will be called when the button2 was pressed 2 times in a short timeframe.
void doubleclick2() {
        button2Node.setProperty("event").send("2click");
} // doubleclick2


// This function will be called once, when the button2 is released after beeing pressed for a long time.
void longPressStart2() {
        button2Node.setProperty("event").send("longclick");
} // longPressStop2


void click3() {
        button3Node.setProperty("event").send("click");
} // click3


// This function will be called when the button3 was pressed 3 times in a short timeframe.
void doubleclick3() {
        button3Node.setProperty("event").send("2click");
} // doubleclick3


// This function will be called once, when the button3 is released after beeing pressed for a long time.
void longPressStart3() {
        button3Node.setProperty("event").send("longclick");
} // longPressStop3

void setup() {
        SonoffDual.setup();
        SonoffDual.setRelays(false, false);
        SonoffDual.setLed(true);


//########## HOMIE stuff
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
        Homie.setLedPin(LEDPIN, HIGH);
        Homie_setFirmware(BOARD_NAME, VERSION);
        Homie.setup();

        blinds.setProperty("position").send("NaN");
//  relay1OnHandler({true,0}, relay1_state?"true":"false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch [relay1.setProperty("on").send("false"); does not work here]
        blinds.setProperty("unit").send("%");
        blinds.setProperty("format").send("integer");
        blinds.advertise("position").settable(blindsMoveHandler);
        blinds.advertise("unit");
        blinds.advertise("format");

        button1Node.setProperty("datatype").send("enum");
        button1Node.setProperty("format").send("click,2click,longclick");
        button1Node.advertise("datatype");
        button1Node.advertise("format");
        button1Node.advertise("event");

        button2Node.setProperty("datatype").send("enum");
        button2Node.setProperty("format").send("click,2click,longclick");
        button2Node.advertise("datatype");
        button2Node.advertise("format");
        button2Node.advertise("event");

        button3Node.setProperty("datatype").send("enum");
        button3Node.setProperty("format").send("click,2click,longclick");
        button3Node.advertise("datatype");
        button3Node.advertise("format");
        button3Node.advertise("event");

  #ifdef ESP8266
        EEPROM.begin(512);
  #endif
        char storedShuttersState[shutters.getStateLength()];
        readInEeprom(storedShuttersState, shutters.getStateLength());
        shutters
        .setOperationHandler(shuttersOperationHandler)
        .setWriteStateHandler(shuttersWriteStateHandler)
        .restoreState(storedShuttersState)
        .setCourseTime(upCourseTime, downCourseTime)
        .onLevelReached(onShuttersLevelReached)
        .begin()
//        .setLevel(30) // Go to 30%
        ;
}

void loop() {

        Homie.loop();
        shutters.loop();
        SonoffDualButton buttonState = SonoffDual.handleButton();
        if (buttonState == SonoffDualButton::SHORT) click1();
        if (buttonState == SonoffDualButton::LONG) longPressStart1();

}
