#include <Homie.h>
#include <functional>
#include <vector>

#include "main.h"

#if BOARD_BUTTONS > 0
BoardButton buttons[BOARD_BUTTONS] = {
        {BoardButton(BUTTONPIN1, "but1", shuttersMoveDownOrStop)}
        ,{BoardButton(BUTTONPIN2, "but2", shuttersMoveUpOrStop)}
        ,{BoardButton(BUTTONPIN3, "but3", shuttersMoveDownOrStop, shuttersMoveUpOrStop)}
};
#endif

#if BOARD_SWITCHES > 0
BoardSwitch switches[BOARD_SWITCHES]= {
        {BoardSwitch(UPRELAY, "lewy")}
        ,{BoardSwitch(DOWNRELAY, "prawy")}
};

#endif

#if BOARD_SHUTTERS > 0
BoardShutter shutters[BOARD_SHUTTERS]= {
        {BoardShutter(UPRELAY, DOWNRELAY, "roleta1", 0)}
//        ,{BoardSwitch(DOWNRELAY, "prawy")}
};

#endif

void shuttersMoveDownOrStop(){
  shutters[0].MoveDownOrStop();
}
void shuttersMoveUpOrStop(){
  shutters[0].MoveUpOrStop();
}

void setup() {



//########## HOMIE stuff

        all_common_setup();
}

void loop() {
        all_common_loop();
        Homie.loop();
}
