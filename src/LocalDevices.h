#ifndef LanDevices_h
#define LanDevices_h

#include <netif/etharp.h>
#include <Ticker.h>
#include "SimpleList.h"
#include "MACAddress.h"

class LanDevice{
	public:
	IPAddress IP;
	MACAddress MAC;
	int64_t lastOnline;
	
	LanDevice(){
		this->lastOnline = esp_timer_get_time()/1000;
	}
	
	LanDevice(const IPAddress IP, const MACAddress MAC){
		this->IP = IP;
		this->MAC = MAC;
		this->lastOnline = esp_timer_get_time()/1000;
	}
	
	LanDevice(const LanDevice &a){
		this->IP = a.IP;
		this->MAC = a.MAC;
		this->lastOnline = a.lastOnline;
	}
	
	LanDevice& operator= (const LanDevice &a){
		this->IP = a.IP;
		this->MAC = a.MAC;
		this->lastOnline = a.lastOnline;
		return *this;
	}
	
	bool operator== (const LanDevice &a){
		return this->IP == a.IP && this->MAC == a.MAC;
	}
};


class LanDevicesClass{
	public:
	LanDevicesClass();
	void begin();
	void rescan();
	void setActiveScan(uint8_t seconds);
	uint8_t getActiveScan();
	void refreshAPdevices();
	void refreshSTAdevices();
	SimpleList<LanDevice> STAdevices;
	SimpleList<LanDevice> APdevices;
	
	private:
	void (*onNewDevice)();
	void (*onLostDevice)();
	static void rescan_callback( void * context );
	uint8_t refreshInterval;
	Ticker refreshTimer;

};
extern LanDevicesClass LanDevices;
  
#endif