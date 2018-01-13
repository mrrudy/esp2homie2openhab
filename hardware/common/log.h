#ifndef common_log_H_
#define common_log_H_


#define DEBUG_PREFIX "DEBUG/"

#ifdef DEBUG_ENABLED
#define Debug(x) Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, x)
#else
#define Debug(x)
#endif

#ifdef DEBUG_ENABLED
char debugbuffer[150];
#define Debugf(...) sprintf(debugbuffer, __VA_ARGS__); Homie.getMqttClient().publish(DEBUG_PREFIX BOARD_NAME, 1, false, debugbuffer)
#else
#define Debugf(...)
#endif

#endif /*common_log_H_*/
