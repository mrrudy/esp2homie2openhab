#define BOARD_NAME "v3"
#define BOARD_FUTURES "1BLI_3BUT_OTA"
#define VERSION "0.94"

#define DOWNRELAY LEFTRELAYPIN
#define UPRELAY RIGHTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)

#include "include/family_common/board.h"
#include "include/common/log.h"
