
#include <WiFi.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <netif/etharp.h>
#include "LocalDevices.h"

#include "Utilities.h"


LanDevicesClass LanDevices;

LanDevicesClass::LanDevicesClass(){
}

void LanDevicesClass::begin() {
	
	refreshInterval=0;
	onDeviceFound = 0;
	onDeviceLost = 0;
	xTaskCreatePinnedToCore(
			reinterpret_cast<TaskFunction_t>(&LanDevicesClass::arp_timer_callback),   /* Function to implement the task */
			"ARP_Timer", /* Name of the task */
			768,      /* Stack size in words */
			NULL,       /* Task input parameter */
			1,          /* Priority of the task */
			NULL,       /* Task handle. */
			APP_CPU_NUM);  /* Core where the task should run */
}

void LanDevicesClass::rescan_callback( void * context ){
	while (((LanDevicesClass*)context)->refreshInterval){
		while (WiFi.status() != WL_CONNECTED){
			delay(666);
		}
		Serial.println(Utilities.timestamp() + " - SCAN START");
		((LanDevicesClass*)context)->rescan();
		Serial.println(Utilities.timestamp() + " - SCAN END");
		for (uint16_t i=0; i<(((LanDevicesClass*)context)->refreshInterval); i++){
			delay(1000);
		}
	}
	vTaskDelete(NULL);
}
void LanDevicesClass::arp_timer_callback( void * context ){
	for(;;){
		etharp_tmr();
		delay(ARP_TMR_INTERVAL);
	}
}

void LanDevicesClass::rescan() {
	
		struct netif *sta_netif;
		sta_netif = netif_list;
		while (sta_netif){
			if (!strncmp(sta_netif->name, "st", 2)) break;
			sta_netif = sta_netif->next;
		}
		if (WiFi.status() == WL_CONNECTED && sta_netif){

			IPAddress subnet = WiFi.subnetMask();
			IPAddress myip = WiFi.localIP();
			IPAddress gwip = WiFi.gatewayIP();

			uint32_t subnetSize = (256 - subnet[0]) * (256 - subnet[1]) * (256 - subnet[2]) * (256 - subnet[3]) - 1;

			IPAddress startIP = WiFi.localIP();
			startIP = (uint32_t)startIP & (uint32_t)subnet;
			startIP[3] += 1;

			// Send ARP requests
			ip4_addr_t toCheck;
			IPAddress IPtoCheck;

			for (int i = 0; i < subnetSize; i++) {
				IPtoCheck = startIP;
				IPtoCheck[0] += (i / 255 / 255 / 255) % 256;
				IPtoCheck[1] += (i / 255 / 255) % 256;
				IPtoCheck[2] += (i / 255) % 256;
				IPtoCheck[3] += i % 256;

				if (IPtoCheck == myip || IPtoCheck == gwip) continue;

				IP4_ADDR(&toCheck, IPtoCheck[0], IPtoCheck[1], IPtoCheck[2], IPtoCheck[3]);

				etharp_request(sta_netif, &toCheck);
				delay(25);
			}
			delay(1000);
		}
		refreshAPdevices();
		refreshSTAdevices();
}

void LanDevicesClass::setActiveScan(uint16_t seconds) {
	refreshInterval = seconds;
	if (seconds) {
		//refreshTimer.attach((float)seconds, &LanDevicesClass::rescan_callback, (void*)(this));
		xTaskCreatePinnedToCore(
			reinterpret_cast<TaskFunction_t>(&LanDevicesClass::rescan_callback),   /* Function to implement the task */
			"Rescan", /* Name of the task */
			2048,      /* Stack size in words */
			(void*)(this),       /* Task input parameter */
			1,          /* Priority of the task */
			NULL,       /* Task handle. */
			APP_CPU_NUM);  /* Core where the task should run */

	}
	// else {
		// refreshTimer.detach();
	// }
}

uint8_t LanDevicesClass::getActiveScan() {
	return refreshInterval;
}

void LanDevicesClass::refreshAPdevices(){
	SimpleList<LanDevice> OldDevices, NewDevices;
	uint16_t i,j;
	for (i=0; i<APdevices.size(); i++){
		OldDevices.add(APdevices.get(i));
	}
	
	APdevices.clear();
	wifi_sta_list_t MACclients;
	tcpip_adapter_sta_list_t IPclients;
	
	if (esp_wifi_ap_get_sta_list(&MACclients) == ESP_OK && MACclients.num)
		if(tcpip_adapter_get_sta_list(&MACclients, &IPclients) == ESP_OK ) {
			for (i=0; i<IPclients.num; i++){
				LanDevice newDevice(IPclients.sta[i].ip.addr, IPclients.sta[i].mac, WIFI_AP);
				APdevices.add(newDevice);
				NewDevices.add(newDevice);
				yield();
			}
		}

	uint8_t change=1;
	while (change){
		change = 0;
		for (i=0; i<NewDevices.size(); i++){
			for (j=0; j<OldDevices.size() && i<NewDevices.size(); j++){
				if (OldDevices.get(j) == NewDevices.get(i)){
					OldDevices.remove(j);
					NewDevices.remove(i);
					change = 1;
				}
				yield();
			}
		}
	}
	
	for (i=0; i<NewDevices.size(); i++){
		if (onDeviceFound) onDeviceFound(NewDevices.get(i));
		yield();
	}
	for (i=0; i<OldDevices.size(); i++){
		if (onDeviceLost) onDeviceLost(OldDevices.get(i));
		yield();
	}
}

void LanDevicesClass::refreshSTAdevices(){
	SimpleList<LanDevice> OldDevices, NewDevices;
	uint16_t i,j;
	for (i=0; i<STAdevices.size(); i++){
		OldDevices.add(STAdevices.get(i));
	}
	
	STAdevices.clear();
	
	struct eth_addr *ret_eth_addr = NULL;
    struct netif *ret_netif = NULL;
    struct ip4_addr *ret_ip_addr = NULL;
    for (int i = 0; etharp_get_entry(i, &ret_ip_addr, &ret_netif, &ret_eth_addr); i++) {
		if (!strncmp(ret_netif->name, "st", 2)){
			LanDevice newDevice(ret_ip_addr->addr, ret_eth_addr->addr, WIFI_STA);
			STAdevices.add(newDevice);
			NewDevices.add(newDevice);
		}
		ret_eth_addr = NULL;
		ret_netif = NULL;
		ret_ip_addr = NULL;
		yield();
    }
	
	uint8_t change=1;
	while (change){
		change = 0;
		for (i=0; i<NewDevices.size(); i++){
			for (j=0; j<OldDevices.size() && i<NewDevices.size(); j++){
				if (OldDevices.get(j) == NewDevices.get(i)){
					OldDevices.remove(j);
					NewDevices.remove(i);
					change = 1;
				}
				yield();
			}
		}
	}
	
	for (i=0; i<NewDevices.size(); i++){
		if (onDeviceFound) onDeviceFound(NewDevices.get(i));
		yield();
	}
	for (i=0; i<OldDevices.size(); i++){
		if (onDeviceLost) onDeviceLost(OldDevices.get(i));
		yield();
	}
}


void LanDevicesClass::setOnDeviceFound(void (*deviceFoundCallback)(LanDevice)){
	this->onDeviceFound = deviceFoundCallback;
}
void LanDevicesClass::setOnDeviceLost(void (*deviceLostCallback)(LanDevice)){
	this->onDeviceLost = deviceLostCallback;
}