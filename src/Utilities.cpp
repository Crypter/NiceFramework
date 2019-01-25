#include "Utilities.h"

UtilitiesClass Utilities;


void UtilitiesClass::disableBrownout(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
}

int64_t UtilitiesClass::sinceTimer(const int64_t millis_value)
{
  return (esp_timer_get_time()/1000) - millis_value;
}

void UtilitiesClass::resetTimer(int64_t &millis_value)
{
  millis_value = esp_timer_get_time()/1000;
}

String UtilitiesClass::uptime(){
  char response[50];
    uint32_t now = esp_timer_get_time()/1000000;
    sprintf(response, "Uptime: %d days, %02d:%02d:%02d", now/60/60/24, (now/60/60)%24, (now/60)%60, now%60);
  return String(response);
}

int64_t UtilitiesClass::uptimeSec(){
  return esp_timer_get_time()/1000000;
}

