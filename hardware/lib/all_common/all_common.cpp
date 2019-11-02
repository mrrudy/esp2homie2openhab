#include <Arduino.h>
#include "all_common.h"
#include "all_common_log.h"
#include "all_common_relay.h"

#define xstr(s) str(s)
#define str(s) #s

#if USERLIB_USE_WATCHDOG_WIFI==true
unsigned long watchdogLastFeed=999999;
Ticker watchdogTick;

void ICACHE_RAM_ATTR ISRwatchdog(void)
{
        if (millis()-watchdogLastFeed > WATCHDOGTIMEOUT)
        {
                Debug("Watchdog resets.");
                ESP.restart();
        }
}
#endif /*USERLIB_USE_WATCHDOG_WIFI*/


void all_common_setup(){
  Debugf("free heap: %d", ESP.getFreeHeap());


#ifdef DEBUG_ENABLED
        Serial.begin(74880);
        Homie.setLedPin(LEDPIN, LEDON);
#else
        Homie.setLedPin(LEDPIN, LEDOFF);
        Homie.disableLogging(); //there is a bug and if this is enabled you get a boot loop
#endif
        Homie.disableResetTrigger();  //if you want to use GPIO0 you need to disable reset trigger as it will reset your configuration when LOW
  Debugf("free heap: %d", ESP.getFreeHeap());

#if USERLIB_USE_WATCHDOG_WIFI==true
        watchdogLastFeed=millis()+5*1000; //give setup some additional time, enought to run its magic
        Debugf("\n\rinitializing watchdog with margin: %lu, current time is: %lu\n\r", watchdogLastFeed, millis());
        watchdogTick.attach(10, ISRwatchdog);
#endif /*USERLIB_USE_WATCHDOG_WIFI*/
Debugf("free heap: %d", ESP.getFreeHeap());
/*
        Homie_setFirmware(xstr(BOARD_FAMILY_NAME) xstr(BOARD_NAME) "-"
#if BOARD_BUTTONS > 0
                          "B" xstr(BOARD_BUTTONS)
#endif
#if BOARD_SHUTTERS > 0
                          "SH" xstr(BOARD_SHUTTERS)
#endif
#if BOARD_SWITCHES > 0
                          "SW" xstr(BOARD_SWITCHES)
#endif
#if USERLIB_USE_WATCHDOG_WIFI==true
                          "WD"
#endif
                          , xstr(VERSION));
*/
//Homie_setFirmware("dupa", "01");
Homie_setFirmware(xstr(BOARD_FAMILY_NAME), xstr(VERSION));
        Homie.setup(); //needs to be after node declaraction if it is dynamic (new)
}

void all_common_loop(){
        Homie.loop();
#if USERLIB_USE_WATCHDOG_WIFI==true
        if(Homie.isConnected()) { watchdogLastFeed=millis(); }//if homie is connected feed the dog

#endif /*USERLIB_USE_WATCHDOG_WIFI*/

#if BOARD_BUTTONS > 0
        for(int i=0; i<BOARD_BUTTONS; i++) {
                buttons[i]._OneButton.tick();
        }
#endif

#if BOARD_SHUTTERS > 0
        for(int i=0; i<BOARD_SHUTTERS; i++) {
                shutters[i].loop();
        }
#endif

}

#if BOARD_SWITCHES > 0

BoardSwitch::BoardSwitch(int GPIO, const char *name)
        :  _GPIO(GPIO)
        , _name(name)
        , _HomieNode(HomieNode(this->_name, "switch"))
{
        _HomieNode.advertise("on").settable(
                [this](const HomieRange& range, const String& value) -> bool {
                relaySwitch(this->_GPIO, value=="true" ? HIGH : LOW);
                this->_HomieNode.setProperty("on").send(value); return true;
        }
                );
}

#endif

#if BOARD_BUTTONS > 0

BoardButton& BoardButton::PropagateHomieStatus(const String& value){
        this->_HomieNode.setProperty("event").send(value);
        return *this;
};
void BoardButton::DefaultHandlerAction(void){
        Debug("No function on MC attached to me (i'm DefaultHandlerAction())");
};

BoardButton::BoardButton(int GPIO, const char *name, button_handler click_handler, button_handler doubleclick_handler, button_handler longclick_handler)
        : _GPIO(GPIO)
        , _name(name)
        , _click_handler(click_handler)
        , _doubleclick_handler(doubleclick_handler)
        , _longclick_handler(longclick_handler)
        , _HomieNode(HomieNode(this->_name, "button"))
        , _OneButton(OneButton(this->_GPIO,true))
{

        _HomieNode.setProperty("datatype").send("enum");
        _HomieNode.setProperty("format").send("click,2click,longclick");
        _HomieNode.advertise("datatype");
        _HomieNode.advertise("format");
        _HomieNode.advertise("event");
        _OneButton.attachClick(
                (
                        [this](void) {
                this->PropagateHomieStatus("click");
                this->_click_handler();
                return;
        }
                )
                );
        _OneButton.attachDoubleClick(
                (
                        [this](void) {
                this->PropagateHomieStatus("2click");
                this->_doubleclick_handler();
                return;
        }
                )
                );
        _OneButton.attachLongPressStart(
                (
                        [this](void) {
                this->PropagateHomieStatus("longclick");
                this->_longclick_handler();
                return;
        }
                )
                );
}
#endif

#if BOARD_SHUTTERS > 0

#include <EEPROM.h>

BoardShutter::BoardShutter(int UpRelayGPIO, int DownRelayGPIO, const char *name, int index, bool powerlineOn, shutters_operation_handler *shuttersOperationHandler)
        : _UpRelayGPIO(UpRelayGPIO)
        ,_DownRelayGPIO(DownRelayGPIO)
        , _name(name)
        , _index(index)
        , _powerlineOn(powerlineOn)
        , _HomieNode(HomieNode(this->_name, "blinds"))
        , _shutters(Shutters())
        , _shuttersCourseTimeUP{"courseTimeUP", "How long (ms) does it take for shutters to run UP"}
        , _shuttersCourseTimeDOWN{"courseTimeDOWN", "How long (ms) does it take for shutters to run DOWN"}
        , _shuttersReverseRelays{"reverseRelays", "Should UP and DOWN relays be switched?"}
        , _shuttersOperationHandler(shuttersOperationHandler)
        , _whichRelayIsWorking(UpRelayGPIO)

{
        pinMode(_UpRelayGPIO, OUTPUT);
        pinMode(_DownRelayGPIO, OUTPUT);

        relaySwitch(_UpRelayGPIO,!_powerlineOn); //whatever we will do first we stop the shutters from moving
        relaySwitch(_DownRelayGPIO,!_powerlineOn); //whatever we will do first we stop the shutters from moving


        _HomieNode.setProperty("position").send("NaN");
        _HomieNode.setProperty("unit").send("%");
        _HomieNode.setProperty("format").send("integer");
        _HomieNode.advertise("unit");
        _HomieNode.advertise("format");
        _HomieNode.advertise("position").settable(
                [this](const HomieRange& range, const String& value) -> bool {
                int new_position_in_percent=0;

                if(!this->_shutters.isIdle()) {   //if we received new position and if blinds are moving stop them first
                        this->_shutters.stop();
                }
                if(value=="STOP") return(true);
                if((value=="UP")||(value=="DOWN")) {
                        new_position_in_percent=(value=="UP") ? 0 : 100;
                }else new_position_in_percent=value.toInt();

                Debugf("received new blinds position request: %d %%", new_position_in_percent);
                this->_shutters.setLevel(new_position_in_percent);
                return true;
        }
                );


        _shuttersCourseTimeUP.setDefaultValue(21000).setValidator([] (long candidate) {
                return candidate >= 1000;
        });
        _shuttersCourseTimeDOWN.setDefaultValue(0).setValidator([] (long candidate) {
                return candidate >= 0;
        });
        _shuttersReverseRelays.setDefaultValue(false).setValidator([] (bool candidate) {
                return (candidate == true) || (candidate == false);
        });

        #ifdef ESP8266
        EEPROM.begin(512);
        #endif
        char storedShuttersState[_shutters.getStateLength()];
        readInEeprom(storedShuttersState, _shutters.getStateLength(), _index);

        _shutters
        .setOperationHandler(
                [this](Shutters* s, ShuttersOperation operation){
                if (_shuttersOperationHandler!=nullptr) {
                        _shuttersOperationHandler(s, operation);
                        return;
                }
                switch (operation) {
                case ShuttersOperation::UP:
                        _whichRelayIsWorking=(_shuttersReverseRelays.get()) ? _DownRelayGPIO : _UpRelayGPIO;
                        Debugf("Shutters going up. Switching relay: %d", _whichRelayIsWorking);
                        relaySwitch(_whichRelayIsWorking,_powerlineOn); //switch the motor relay to the line with up shutters
                        break;
                case ShuttersOperation::DOWN:
                        _whichRelayIsWorking=(_shuttersReverseRelays.get()) ? _UpRelayGPIO : _DownRelayGPIO;
                        Debugf("Shutters going down. Switching relay: %d", _whichRelayIsWorking);
                        relaySwitch(_whichRelayIsWorking,_powerlineOn); //switch the motor relay to the line with up shutters
                        break;
                case ShuttersOperation::HALT:
                        Debug("Shutters stop.");
                        relaySwitch(_whichRelayIsWorking,!_powerlineOn); //whatever we will do first we stop the shutters from moving
                        //                relaySwitch(DOWNRELAY,!ENGINELINE); //whatever we will do first we stop the shutters from moving
                        break;
                };
        }
                )
        .setWriteStateHandler(
                [this](Shutters* shutters, const char* state, byte length){
                for (byte i = 0; i < length; i++) {
                        EEPROM.write((_index * length) + i, state[i]);
        #ifdef ESP8266
                        EEPROM.commit();
        #endif
                }
        }
                )
        .restoreState(storedShuttersState)
        .setCourseTime(_shuttersCourseTimeUP.get(), _shuttersCourseTimeDOWN.get())
        .onLevelReached([this](Shutters* shutters, byte level) -> void {
                if (level%5==0) this->_HomieNode.setProperty("position").send(String(level));
        })
        .begin()
//        .setLevel(30) // Go to 30%
        ;
}
void BoardShutter::MoveDownOrStop(){
        if(!_shutters.isIdle()) { //if blinds are moving stop them
                _shutters.stop();
        }
        else {
                _shutters.setLevel((int)100); //move them 100% (down)
        }
}

void BoardShutter::MoveUpOrStop(){
        if(!_shutters.isIdle()) { //if blinds are moving stop them
                _shutters.stop();
        }
        else {
                _shutters.setLevel((int)0); //move them 100% (down)
        }
}

void BoardShutter::loop(){
        _shutters.loop();
}

void readInEeprom(char* dest, byte length, int eepromOffset) {
        for (byte i = 0; i < length; i++) {
                dest[i] = EEPROM.read((eepromOffset * length) + i);
        }
        Debugf("read from eeprom position: %d %%", (int) dest);
}


#endif /*BOARD_SHUTTERS*/
