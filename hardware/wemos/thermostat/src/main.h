#define BOARD_NAME "empty"
#define BOARD_FUTURES "1NOTHING_3NADA_OTA"
#define VERSION "0.1"

#include "board.h"
#include "log.h"

#define DHTPIN D4
#define RELAYPIN D1

#define RELAY_NC_OFF true  //if the turning off configuration is connected to NC port

#define DEFAULT_TEMPERATURE_INTERVAL 5
