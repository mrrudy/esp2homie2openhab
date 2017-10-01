#include <Homie.h>
#include "OneButton.h"

#define BOARD_NAME "suplav3"
#define BOARD_FUTURES "1BLINDS_3BUTTON_OTA"
#define DEBUG_PREFIX "DEBUG/"
#define DEBUG_ENABLED 1
#define RELAYPIN1 5
#define RELAYPIN2 13
#define BUTTONPIN1 14
#define BUTTONPIN2 12
#define BUTTONPIN3 4
#define LEFT HIGH
#define RIGHT LOW

unsigned long timer=0; //timer that marks time in future till when the blinds should roll; if 0 means there is no movement
bool up_direction=true; //direction of the blinds movement
unsigned long move_start=0; //stores time when the movement started
const unsigned long fullmove=10 * 1000UL; //how long does it take for full blinds move
long blinds_position=0; //position of blinds as time in ms from full open; 0 means position is unknown
const int REPORT_INTERVAL = 1 * 1000UL; //how often I should report the blinds status during move
unsigned long lastReportSent=0;

OneButton button1(BUTTONPIN1, true);
OneButton button2(BUTTONPIN2, true);
OneButton button3(BUTTONPIN3, true);

HomieNode button1Node("b1", "button");
HomieNode button2Node("b2", "button");
HomieNode button3Node("b3", "button");

bool relay1_state=false; //holds the state in which the relay1 is
bool relay2_state=false; //holds the state in which the relay2 is

HomieNode blinds("blinds1", "blinds");

void click1() {
  #if DEBUG_ENABLED
  Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "button1 click");
  #endif
  button1Node.setProperty("event").send("click");  //sending this but be sure not to double react on it as below code already enables relay
  if(timer!=0){ //if blinds are moving stop them
    moveBlindsSTOP();
  }
  else {
    moveBlindsUP(1.2* fullmove); //move them for 120% the time it takes the blinds make a full cycle (just in case)
  }
} // click1

bool blindsMoveHandler(const HomieRange& range, const String& value) {
int new_position_in_percent=0;

if(timer!=0){ //if blinds are moving stop them first
  moveBlindsSTOP();
}
if(value=="STOP")return(true);
if((value=="UP")||(value=="DOWN")) {
    new_position_in_percent=(value=="UP")?0:100;
}else new_position_in_percent=value.toInt();

    #if DEBUG_ENABLED
      char buffer[100];
      sprintf(buffer, "received new blinds position request: %d %%", new_position_in_percent);
      Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
    #endif
    
    moveBlindsToPosition((unsigned long)(fullmove*new_position_in_percent)/100);
}

void moveBlindsToPosition(unsigned long req_position){
  if(req_position<=1){
    moveBlindsUP(1.2* fullmove);
    return;
  }
  if(req_position>=fullmove){
    moveBlindsDOWN(1.2* fullmove);
    return;
  }
  if(blinds_position){
    if(req_position<blinds_position){
      moveBlindsUP(blinds_position-req_position);
      return;
    }
    else {
      moveBlindsDOWN(req_position-blinds_position);
      return;
    }
  }
  else{
    #if DEBUG_ENABLED
      Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "not implemented moving to a position when current position is unknown, try moving blinds full open/close first");
    #endif
    //TODO: implement first full open/close then going to a position
  }
}

void moveBlindsUP(unsigned long how_long){
  #if DEBUG_ENABLED
    Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "moving blinds up");
  #endif

  relaySwitch(RELAYPIN1,LEFT); //stop the blinds
  up_direction=true; //blinds are going up
  relaySwitch(RELAYPIN2,LEFT);
  moveBlindsSTART(how_long);
}
void moveBlindsDOWN(unsigned long how_long){
  #if DEBUG_ENABLED
    Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "moving blinds down");
  #endif

  relaySwitch(RELAYPIN1,LEFT); //stop the blinds
  up_direction=false; //blinds are going up
  relaySwitch(RELAYPIN2,RIGHT);
  moveBlindsSTART(how_long);  
}
void moveBlindsSTOP(){
  #if DEBUG_ENABLED
    char buffer[100];
  #endif
  #if DEBUG_ENABLED
    Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, "stopping blinds");
  #endif
  
  relaySwitch(RELAYPIN1,LEFT);
  
  unsigned long delta=millis()-move_start; //calculate the delta of movement
  timer=0; //reset the timer to show blinds are not moving
  if(delta>=fullmove) { //if we moved long enought to reach the end
     blinds_position=(up_direction ? 1 : fullmove); //set the position to up if we moved up or to end if we moved down
     #if DEBUG_ENABLED
      sprintf(buffer, "new position after fullmove is: %lu", blinds_position);
      Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
    #endif
  }
  else if(blinds_position){ //if the position of blinds was known
    #if DEBUG_ENABLED
      sprintf(buffer, "position was: %lu delta is: %lu", blinds_position, delta);
      Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
    #endif
     blinds_position+=(up_direction ? (-delta) : delta); //substract the delta if blinds were going up, add if they were going down
     if(blinds_position<=0) blinds_position=1;
     if(blinds_position>fullmove) blinds_position=fullmove;
    #if DEBUG_ENABLED
      sprintf(buffer, "new position is: %lu", blinds_position);
      Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
    #endif
  }
  blinds.setProperty("position").send(blinds_position ? String((blinds_position*100/fullmove)):"NaN"); //send the position
}

void moveBlindsSTART(unsigned long how_long){
  relaySwitch(RELAYPIN1,RIGHT);
  move_start=millis(); //record when movement started;
  timer=move_start + how_long; //set timer 
  #if DEBUG_ENABLED
    char buffer[100];
    sprintf(buffer, "move started at: %lu should stop at: %lu", move_start, timer);
    Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
  #endif
}

bool relaySwitch(int whichRelay, int channel) {
  #if DEBUG_ENABLED
    char buffer[50];
  #endif
  digitalWrite(whichRelay, channel); //HIGH/LOW = LEFT/RIGHT
  #if DEBUG_ENABLED
    sprintf(buffer, "changing switch on pin no. %d to state: %d", whichRelay, channel);
    Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, buffer);
  #endif
  return (true);
}


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
  Homie_setFirmware(BOARD_NAME "-" BOARD_FUTURES, "1.0.2");
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

}

void loop() {

  Homie.loop();
  button1.tick();
  button2.tick();
  button3.tick();
  if(timer!=0 && timer<=millis()){ //if we reached a point in future when the blinds should stop moving
      moveBlindsSTOP(); //we stop them
  }
  if (timer!=0 && (millis() - lastReportSent >= REPORT_INTERVAL) && (blinds_position)) {
    unsigned long delta=millis()-move_start; //calculate the delta of movement
    long current_blinds_position=blinds_position+(up_direction ? (-delta) : delta); //substract the delta if blinds were going up, add if they were going down
    if(current_blinds_position<=0) current_blinds_position=1;
    if(current_blinds_position>fullmove) current_blinds_position=fullmove;
    blinds.setProperty("position").send(String(current_blinds_position*100/fullmove)); //send the position
    lastReportSent=millis(); 
  }
}




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
  if(timer!=0){ //if blinds are moving stop them
    moveBlindsSTOP();
  }
  else {
    moveBlindsDOWN(2*fullmove); //move them for twice the time it takes the blinds make a full cycle (just in case)
  }

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

