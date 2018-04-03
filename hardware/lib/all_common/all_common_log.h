#ifndef common_log_H_
#define common_log_H_
#include <Homie.h>

#define DEBUG_PREFIX "DEBUG/"

#ifdef DEBUG_ENABLED
#define Debug(x) Homie.getMqttClient().publish(DEBUG_PREFIX, 1, false, x)
#else
#define Debug(x)
#endif

#ifdef DEBUG_ENABLED

#define Debugf(...) char debugbuffer[150]; sprintf(debugbuffer, __VA_ARGS__); Homie.getMqttClient().publish(DEBUG_PREFIX, 1, false, debugbuffer)
#else
#define Debugf(...)
#endif

#endif /*common_log_H_*/
