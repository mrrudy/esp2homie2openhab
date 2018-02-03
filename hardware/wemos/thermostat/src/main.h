#define BOARD_NAME "empty"
#define BOARD_FUTURES "1NOTHING_3NADA_OTA"
#define VERSION "0.4"

#include "board.h"
#include "log.h"

#define DHTPIN D4
#define RELAYPIN D1

#define RELAY_NC_OFF true  //if the turning off configuration is connected to NC port

#define DEFAULT_TEMPERATURE_INTERVAL 5
#define DEFAULT_DESIRED_TEMPERATURE 21
#define DEFAULT_HISTERESIS 0.5
