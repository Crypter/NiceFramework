#include "Utilities.h"
#include <WiFi.h>
#include <rom/rtc.h>

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

String UtilitiesClass::getResetReason(){
	RESET_REASON rtc_reset_reason = rtc_get_reset_reason(PRO_CPU_NUM);
	String reason;
	
	switch (rtc_reset_reason) {
		case 1  : reason = "Vbat power on reset"; break;
		case 3  : reason = "Software reset digital core";break;
		case 4  : reason = "Legacy watch dog reset digital core";break;
		case 5  : reason = "Deep Sleep reset digital core";break;
		case 6  : reason = "Reset by SLC module, reset digital core";break;
		case 7  : reason = "Timer Group0 Watch dog reset digital core";break;
		case 8  : reason = "Timer Group1 Watch dog reset digital core";break;
		case 9  : reason = "RTC Watch dog Reset digital core";break;
		case 10 : reason = "Instrusion tested to reset CPU";break;
		case 11 : reason = "Time Group reset CPU";break;
		case 12 : reason = "Software reset CPU";break;
		case 13 : reason = "RTC Watch dog Reset CPU";break;
		case 14 : reason = "for APP CPU, reseted by PRO CPU";break;
		case 15 : reason = "Brownout";break;
		case 16 : reason = "RTC Watch dog reset digital core and rtc module";break;
		default : reason = "Unknown";
	}
	return reason;
}

void UtilitiesClass::timesync( void * parameter ) {
	do {
		while (WiFi.status() != WL_CONNECTED) delay(500);
		configTime(UtilitiesClass::GMT*3600, 0000, "pool.ntp.org", "time.nist.gov");
		delay(UtilitiesClass::NTP_delay*60*1000);
	} while (UtilitiesClass::NTP_delay);
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
		APP_CPU_NUM);  /* Core where the task should run */
	}
}

void UtilitiesClass::NTPUpdate(int8_t GMT){
	UtilitiesClass::GMT = GMT;
	xTaskCreatePinnedToCore(
		reinterpret_cast<TaskFunction_t>(&UtilitiesClass::timesync),   /* Function to implement the task */
		"NTP_once", /* Name of the task */
		3000,      /* Stack size in words */
		(void*)(this),       /* Task input parameter */
		1,          /* Priority of the task */
		NULL,       /* Task handle. */
		APP_CPU_NUM);  /* Core where the task should run */
}