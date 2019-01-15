#ifndef LanDevices_h
#define LanDevices_h

#include <netif/etharp.h>
#include <Ticker.h>
#include "MACAddress.h"
#include "LinkedList.h"

class LanDevice{
	public:
	IPAddress IP;
	MACAddress MAC;
	int64_t lastOnline;
	
	LanDevice(){
		this->lastOnline = esp_timer_get_time()/1000;
	}
	
	LanDevice(IPAddress IP, MACAddress MAC){
		this->IP = IP;
		this->MAC = MAC;
		this->lastOnline = esp_timer_get_time()/1000;
	}
	
	LanDevice (const LanDevice &a){
		this->IP = a.IP;
		this->MAC = a.MAC;
		this->lastOnline = a.lastOnline;
	}
	
	LanDevice& operator= (const LanDevice &a){
		this->IP = a.IP;
		this->MAC = a.MAC;
		this->lastOnline = a.lastOnline;
	}
	
	bool operator== (const LanDevice &a){
		return this->IP == a.IP && this->MAC == a.MAC;
	}
};


class LanDevicesClass{
	public:
	void begin();
	void refresh();
	void setActiveScan(uint8_t seconds);
	uint8_t getActiveScan();
	const LinkedList<LanDevice> getAPdevices();
	const LinkedList<LanDevice> getSTAdevices();
	
	private:
	LinkedList<LanDevice> STAdevices;
	LinkedList<LanDevice> APdevices;
	void (*onNewDevice)();
	void (*onLostDevice)();
	
	uint8_t refreshInterval;
	Ticker refreshTimer;

};
extern LanDevicesClass LanDevices;
  
#endif