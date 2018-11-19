
#define DOWNRELAY RIGHTRELAYPIN
#define UPRELAY LEFTRELAYPIN
#define ENGINELINE LEFT //to which side the cable that connects power to the SHUTTERELAY is connected (should be the Normally Open channel)


#include "all_common_log.h"
#include "all_common_relay.h"
#include "userconf.h"
#include "all_common.h"


void shuttersMoveDownOrStop(void);
void shuttersMoveUpOrStop(void);
