#define BOARD_NAME "v3"
#define BOARD_FUTURES "1BLI_3BUT_OTA"
#define VERSION "0.95"

#define DOWNRELAY RIGHTRELAYPIN
#define UPRELAY LEFTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)

#include "board.h"
#include "all_common_log.h"
#include "all_common_relay.h"
