#include <Homie.h>
#include <OneButton.h>

#include "main.h"
//#include "common/all_common_relay.h"

/*bool relaySwitch(int whichRelay, int channel) { //TODO repair include from common cpp and move it there
        digitalWrite(whichRelay, channel);
        return (true);
}*/

OneButton button1(BUTTONPIN, true);
OneButton button2(J1PIN5, true);

HomieNode button1Node("b1", "button");
HomieNode button2Node("b2", "button");

HomieNode relay1("center", "switch");


bool relayOnHandler(const HomieRange& range, const String& value) {
    relaySwitch(RELAYPIN,value=="true" ? HIGH : LOW);
    relay1.setProperty("on").send(value); //
    return true;
}


// This function will be called when the button1 was pressed shortly.
void click1() {
        Debug("button1 click");
        button1Node.setProperty("event").send("click"); //sending this but be sure not to double react on it as below code already enables relay
//relaySwitch(RELAYPIN,HIGH);
} // click1

// This function will be called when the button2 was pressed shortly.
void click2() {
        Debug("button2 click");
        button2Node.setProperty("event").send("click"); //sending this but be sure not to double react on it as below code already enables relay
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


void setup() {

  pinMode(RELAYPIN, OUTPUT);
relaySwitch(RELAYPIN,false);
//########## HOMIE stuff
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
        Homie.setLedPin(LEDPIN, LOW);
        Homie_setFirmware(BOARD_NAME, VERSION);
        Homie.setup();

        relay1.advertise("on").settable(relayOnHandler);


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

        button1.attachClick(click1);

}

void loop() {

        Homie.loop();
        button1.tick();
        button2.tick();
}
