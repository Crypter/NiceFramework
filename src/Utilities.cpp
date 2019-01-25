#include "Utilities.h"
#include <WiFi.h>
UtilitiesClass Utilities;
uint16_t UtilitiesClass::NTP_delay = 0;
int8_t UtilitiesClass::GMT = 0;


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

void UtilitiesClass::timesync( void * parameter ) {
	while  (UtilitiesClass::NTP_delay) {
		while (WiFi.status() != WL_CONNECTED) delay(500);
		configTime(UtilitiesClass::GMT*3600, 0000, "pool.ntp.org");
		delay(UtilitiesClass::NTP_delay*60*1000);
	}
	vTaskDelete(NULL);
}

void UtilitiesClass::NTPAutoUpdate(int8_t GMT, uint16_t minutes){
	UtilitiesClass::GMT = GMT;
	UtilitiesClass::NTP_delay = minutes;
	if (minutes){
		xTaskCreatePinnedToCore(
		reinterpret_cast<TaskFunction_t>(&UtilitiesClass::timesync),   /* Function to implement the task */
		"NTP", /* Name of the task */
		3000,      /* Stack size in words */
		(void*)(this),       /* Task input parameter */
		1,          /* Priority of the task */
		NULL,       /* Task handle. */
		1);  /* Core where the task should run */
	}
}

void UtilitiesClass::NTPUpdate(int8_t GMT){
	UtilitiesClass::GMT = GMT;
	if (WiFi.status() == WL_CONNECTED) configTime(UtilitiesClass::GMT*3600, 0000, "pool.ntp.org");
}