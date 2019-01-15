#include <WiFi.h>

#include <netif/etharp.h>
#include <Ticker.h>
#include "LocalDevices.h"

LanDevicesClass LanDevices;

void LanDevicesClass::begin() {
	refreshInterval=0;
}

void LanDevicesClass::refresh() {
    if (WiFi.status() != WL_CONNECTED || !netif_default)  return;

    /* if (sinceTimer(arptimer) > ARP_TMR_INTERVAL) {
      etharp_tmr();
      resetTimer(arptimer);
    } */

    IPAddress subnet = WiFi.subnetMask();
    IPAddress myip = WiFi.localIP();
    IPAddress gwip = WiFi.gatewayIP();

    uint32_t subnetSize = (256 - subnet[0]) * (256 - subnet[1]) * (256 - subnet[2]) * (256 - subnet[3]) - 1;

    IPAddress startIP = WiFi.localIP();
    startIP[3] += 1;
    startIP = (uint32_t)startIP & (uint32_t)subnet;

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

      if (netif_default) etharp_request(netif_default, &toCheck);
      else break;
      yield();
    }
}

void LanDevicesClass::setActiveScan(uint8_t seconds) {
	refreshInterval = seconds;
	if (seconds) refreshTimer.attach((float)seconds, reinterpret_cast<Ticker::callback_t>(&LanDevicesClass::refresh));
	else refreshTimer.detach();
}

uint8_t LanDevicesClass::getActiveScan() {
	return refreshInterval;
}


const LinkedList<LanDevice> LanDevicesClass::getAPdevices(){
	APdevices.clear();
	wifi_sta_list_t MACclients;
	tcpip_adapter_sta_list_t IPclients;
    /*if(esp_wifi_ap_get_sta_list(&clients) == ESP_OK) {
		for (int i=0; i<clients.num; i++){
			LanDevice newDevice;
			newDevice.MAC=clients[i].mac;
			APdevices.add(newDevice);
		}
    }*/
	
	if(tcpip_adapter_get_sta_list(&MACclients, &IPclients) == ESP_OK ) {
		for (int i=0; i<IPclients.num; i++){
		LanDevice newDevice(IPclients.sta[i].ip.addr, IPclients.sta[i].mac);
		APdevices.add(newDevice);
		}
	}
	return APdevices;
}

const LinkedList<LanDevice> LanDevicesClass::getSTAdevices(){
	STAdevices.clear();
	struct eth_addr *ret_eth_addr = NULL;
    struct netif *ret_netif = NULL;
    struct ip4_addr *ret_ip_addr = NULL;

    for (int i = 0; etharp_get_entry(i, &ret_ip_addr, &ret_netif, &ret_eth_addr); i++) {
		LanDevice newDevice(ret_ip_addr->addr, ret_eth_addr->addr);

		ret_eth_addr = NULL;
		ret_netif = NULL;
		ret_ip_addr = NULL;
    }
	return STAdevices;
}