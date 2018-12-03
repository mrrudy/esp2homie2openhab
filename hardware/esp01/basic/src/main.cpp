#include "main.h"

#if BOARD_BUTTONS > 0
BoardButton buttons[BOARD_BUTTONS] = {
        {BoardButton(10, "but1")}
//        ,{BoardButton(BUTTONPIN2, "but2", shuttersMoveUpOrStop)}

};
#endif

#if BOARD_SWITCHES > 0
BoardSwitch switches[BOARD_SWITCHES]= {
        {BoardSwitch(LEFTRELAYPIN, "lewy")}
//        ,{BoardSwitch(DOWNRELAY, "prawy")}
};

#endif

#if BOARD_SHUTTERS > 0
BoardShutter shutters[BOARD_SHUTTERS]= {
        {BoardShutter(UPRELAY, DOWNRELAY, "roleta1", 0)}
//        ,{BoardSwitch(DOWNRELAY, "prawy")}
};

#endif

void shuttersMoveDownOrStop(){

}
void shuttersMoveUpOrStop(){

}

void setup() {

        all_common_setup();
}

void loop() {
        all_common_loop();

}
