#include "include/family_common/board.h"
#define BOARD_NAME "suplav3"
#define BOARD_FUTURES "1BLINDS_3BUTTON_OTA"
#define VERSION "0.92"
#define DEBUG_PREFIX "DEBUG/"
#define DEBUG_ENABLED 1
#define LEFTRELAYPIN 5
#define RIGHTRELAYPIN 13
#define BUTTONPIN1 14
#define BUTTONPIN2 12
#define BUTTONPIN3 4
#define LEFT HIGH
#define RIGHT LOW


#define UPRELAY LEFTRELAYPIN
#define DOWNRELAY RIGHTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)
//#define UPLINE LEFT //to which side of the SHUTTERRELAY the up motor is connected to

#ifdef DEBUG_ENABLED
#define Debug(x) Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, x)
#else
#define Debug(x)
#endif

#ifdef DEBUG_ENABLED
char debugbuffer[150];
#define Debugf(...) sprintf(debugbuffer, __VA_ARGS__); Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, debugbuffer)
#else
#define Debugf(x)
#endif

bool relaySwitch(int whichRelay, int channel); //turns relay to the demanded channel
