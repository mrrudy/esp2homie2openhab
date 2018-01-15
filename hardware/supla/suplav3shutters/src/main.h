<<<<<<< HEAD
#define BOARD_NAME "suplav3"
#define BOARD_FUTURES "1BLINDS_3BUTTON_OTA"
#define VERSION "0.93"
#define DEBUG_PREFIX "DEBUG/"
#define DEBUG_ENABLED 1
#define LEFTRELAYPIN 5
#define RIGHTRELAYPIN 13
#define BUTTONPIN1 14
#define BUTTONPIN2 12
#define BUTTONPIN3 4
#define LEFT HIGH
#define RIGHT LOW
=======
#define BOARD_NAME "shutters"
#define BOARD_FUTURES "1SHUTTERS_3BUTTON_OTA"
#define VERSION "0.93"
>>>>>>> dc18387f5d28b16acbfd8e817b92c8bceef7a5fc

#define DOWNRELAY LEFTRELAYPIN
#define UPRELAY RIGHTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)

#include "include/family_common/board.h"
#include "include/common/log.h"
