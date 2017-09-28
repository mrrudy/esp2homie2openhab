#include <Homie.h>
#include "OneButton.h"

#define BOARD_NAME "suplav3"
#define BOARD_FUTURES "2RELAY_2BUTTON_OTA"
#define DEBUG_PREFIX "DEBUG/"
#define DEBUG_ENABLED 1
#define RELAYPIN1 5
#define RELAYPIN2 13
#define BUTTONPIN1 14
#define BUTTONPIN2 12
#define BUTTONPIN3 4

OneButton button1(BUTTONPIN1, true);
OneButton button2(BUTTONPIN2, true);
OneButton button3(BUTTONPIN3, true);

HomieNode button1Node("b1", "button");
HomieNode button2Node("b2", "button");
HomieNode button3Node("b3", "button");

bool relay1_state=false; //holds the state in which the relay1 is
bool relay2_state=false; //holds the state in which the relay2 is

HomieNode relay1("r1", "switch");
HomieNode relay2("r2", "switch");



void setup() {
  pinMode(RELAYPIN1, OUTPUT);
  pinMode(RELAYPIN2, OUTPUT);

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
  Homie.disableLogging();  //there is a bug and if this is enabled you get a boot loop
  Homie.setLedPin(15, HIGH);
  Homie_setFirmware(BOARD_NAME "-" BOARD_FUTURES, "1.0.5");
  Homie.setup();
  
  relay1.advertise("on").settable(relay1OnHandler);
  relay1OnHandler({true,0}, relay1_state?"true":"false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch [relay1.setProperty("on").send("false"); does not work here]
  relay2.advertise("on").settable(relay2OnHandler);
  relay2OnHandler({true,0}, relay2_state?"true":"false"); //sets the default state of the switch. it does not report to mqtt! TODO: report to the mqtt initial state of the switch

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

}

void loop() {
unsigned long now = millis();
  Homie.loop();
  button1.tick();
  button2.tick();
  button3.tick();
}

void click1() {
  button1Node.setProperty("event").send("click");  //sending this but be sure not to double react on it as below code already enables relay
  relay1OnHandler({true,0}, relay1_state?"false":"true"); //reverse the switch state
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button1 click");
  #endif
} // click1


// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
  button1Node.setProperty("event").send("2click");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button1 doubleclick");
  #endif
} // doubleclick1


// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStart1() {
  button1Node.setProperty("event").send("longclick");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button1 longpress stop");
  #endif
} // longPressStop1

void click2() {
  button2Node.setProperty("event").send("click");  //sending this but be sure not to double react on it as below code already enables relay
  relay2OnHandler({true,0}, relay2_state?"false":"true"); //reverse the switch state
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button2 click");
  #endif
} // click2


// This function will be called when the button2 was pressed 2 times in a short timeframe.
void doubleclick2() {
  button2Node.setProperty("event").send("2click");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button2 doubleclick");
  #endif
} // doubleclick2


// This function will be called once, when the button2 is released after beeing pressed for a long time.
void longPressStart2() {
  button2Node.setProperty("event").send("longclick");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button2 longpress stop");
  #endif
} // longPressStop2


bool relay1OnHandler(const HomieRange& range, const String& value) {
  int mask = RELAYPIN1;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay1.setProperty("on").send(value); // TODO: change to self
    relay1_state=(value=="true")?true:false; //TODO: move to realySwitch and move all to structure
    return true;
  }
  else return false;
}

bool relay2OnHandler(const HomieRange& range, const String& value) {
  int mask = RELAYPIN2;  //relay number 1 (left)
  if(relaySwitch(mask, value)) {
    relay2.setProperty("on").send(value); // TODO: change to self
    relay2_state=(value=="true")?true:false; //TODO: move to realySwitch and move all to structure
    return true;
  }
  else return false;
}

bool relaySwitch(int mask, const String& value) {
  #if DEBUG_ENABLED
   char buffer[50];
  #endif
  if (value != "true" && value != "false") return false;
  bool on = (value == "true");
  if(on) {
    digitalWrite(mask, HIGH); //turns on relay
  }
  else {
    digitalWrite(mask, LOW);
  }
  #if DEBUG_ENABLED
  sprintf(buffer, "changing switch no. %d to state: %s", mask, on ? "ON" : "OFF");
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
  #endif
  return (true);
}


void click3() {
  button3Node.setProperty("event").send("click");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button3 click");
  #endif
} // click3


// This function will be called when the button3 was pressed 3 times in a short timeframe.
void doubleclick3() {
  button3Node.setProperty("event").send("2click");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button3 doubleclick");
  #endif
} // doubleclick3


// This function will be called once, when the button3 is released after beeing pressed for a long time.
void longPressStart3() {
  button3Node.setProperty("event").send("longclick");
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button3 longpress stop");
  #endif
} // longPressStop3

