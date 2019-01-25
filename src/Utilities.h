#ifndef NiceUtilities_h
#define NiceUtilities_h

#include <Arduino.h>
//Brownout bullshit
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
//Brownout bullshit

class UtilitiesClass{
	public:
void disableBrownout();

int64_t sinceTimer(const int64_t millis_value);

void resetTimer(int64_t &millis_value);

String uptime();

int64_t uptimeSec();

};
extern UtilitiesClass Utilities;
  
#endif