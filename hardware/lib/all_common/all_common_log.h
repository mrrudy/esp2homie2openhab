#ifndef all_common_log_H_
#define all_common_log_H_
#include <Homie.h>

#define DEBUG_PREFIX "DEBUG/"

#ifdef DEBUG_ENABLED

extern char _all_common_log_debugbuffer[150];

#define Debug(x) Homie.getMqttClient().publish(DEBUG_PREFIX, 1, false, x); Serial.println(x);
#else
#define Debug(x)
#endif

#ifdef DEBUG_ENABLED

#define Debugf(...) sprintf(_all_common_log_debugbuffer, __VA_ARGS__); Homie.getMqttClient().publish(DEBUG_PREFIX, 1, false, _all_common_log_debugbuffer); Serial.println(_all_common_log_debugbuffer)
#else
#define Debugf(...)
#endif

#endif /*all_common_log_H_*/
