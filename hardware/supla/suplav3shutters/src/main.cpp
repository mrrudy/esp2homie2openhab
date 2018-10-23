#include <Homie.h>
#include <OneButton.h>
#include <EEPROM.h>
#include <Shutters.h>
#include <Ticker.h>

#include "main.h"

Ticker watchdogTick;

unsigned long watchdogLastFeed=999999; //ensure that the watchdog does not bite before setup
#define watchdogTimeout 60*1000 //time after which the watchdog resets

void ICACHE_RAM_ATTR ISRwatchdog(void)
{
  Serial.printf("Watchdog check: %lu\n\r", millis());
  if (millis()-watchdogLastFeed > watchdogTimeout)
  {
    Serial.println("Watchdog resets.");
    Serial.printf("Current time: %lu, last feed: %lu, difference: %lu\n\r", millis(), watchdogLastFeed, millis()-watchdogLastFeed);
    ESP.restart();
  }
}

bool relaySwitch(int whichRelay, int channel) {
        digitalWrite(whichRelay, channel);
        return (true);
}

const byte eepromOffset = 0;
//const unsigned long upCourseTime = 21 * 1000;
//const unsigned long downCourseTime = 21 * 1000;
const float calibrationRatio = 0.2;

OneButton button1(BUTTONPIN1, true);
OneButton button2(BUTTONPIN2, true);
OneButton button3(BUTTONPIN3, true);

HomieNode button1Node("b1", "button");
HomieNode button2Node("b2", "button");
HomieNode button3Node("b3", "button");

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
        Debugf("read from eeprom position: %d %%", dest);
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
        pinMode(UPRELAY, OUTPUT);
        pinMode(DOWNRELAY, OUTPUT);

        relaySwitch(UPRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
        relaySwitch(DOWNRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving

        button1.attachClick(click1);
        button1.attachDoubleClick(doubleclick1);
        button1.attachLongPressStart(longPressStart1);
        button2.attachClick(click2);
        button2.attachDoubleClick(doubleclick2);
        button2.attachLongPressStart(longPressStart2);
        button3.attachClick(click3);
        button3.attachDoubleClick(doubleclick3);
        button3.attachLongPressStart(longPressStart3);


//########## HOMIE stuff
#ifdef DEBUG_ENABLED
        Serial.begin(74880);
#else
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
#endif
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
        .setCourseTime(shuttersCourseTimeUP.get(), shuttersCourseTimeDOWN.get())
        .onLevelReached(onShuttersLevelReached)
        .begin()
//        .setLevel(30) // Go to 30%
        ;
//        Debugf("Setting course of shutters to %d", shuttersClosingTime.get());
    watchdogLastFeed=millis()+5*1000; //give setup some additional time, enought to run its magic
    Serial.printf("initializing watchdog with margin: %lu, current time is: %lu\n\r", watchdogLastFeed, millis());
    watchdogTick.attach(10, ISRwatchdog);
}

void loop() {
        if(Homie.isConnected()) { watchdogLastFeed=millis(); }//if homie is connected feed the dog
        Homie.loop();
        button1.tick();
        button2.tick();
        button3.tick();
        shutters.loop();
}
