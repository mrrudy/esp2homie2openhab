#define BOARD_NAME "shutters"
#define BOARD_FUTURES "1SHUTTERS_3BUTTON_OTA"
#define VERSION "0.93"

#define UPRELAY LEFTRELAYPIN
#define DOWNRELAY RIGHTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)

#include "include/family_common/board.h"
#include "include/common/log.h"
