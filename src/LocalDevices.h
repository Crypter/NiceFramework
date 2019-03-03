#ifndef LanDevices_h
#define LanDevices_h

#include "SimpleList.h"
#include "MACAddress.h"

class LanDevice{
	public:
	IPAddress IP;
	MACAddress MAC;
	int64_t lastOnline;
	uint8_t interface;
	
	LanDevice(){
		this->lastOnline = esp_timer_get_time()/1000;
	}
	
	LanDevice(const IPAddress IP, const MACAddress MAC, uint8_t interface){
		this->IP = IP;
		this->MAC = MAC;
		this->lastOnline = esp_timer_get_time()/1000;
		this->interface = interface;
	}
	
	LanDevice(const LanDevice &a){
		this->IP = (uint32_t)a.IP;
		this->MAC = (uint64_t)a.MAC;
		this->lastOnline = a.lastOnline;
		this->interface = a.interface;
	}
	
	LanDevice& operator= (const LanDevice &a){
		this->IP = a.IP;
		this->MAC = a.MAC;
		this->lastOnline = a.lastOnline;
		this->interface = a.interface;
		return *this;
	}
	
	bool operator== (const LanDevice &a){
		return this->IP == a.IP && this->MAC == a.MAC && this->interface == a.interface;
	}
};


class LanDevicesClass{
	public:
	LanDevicesClass();
	void begin();
	void rescan();
	void setActiveScan(uint16_t seconds);
	uint8_t getActiveScan();
	void refreshAPdevices();
	void refreshSTAdevices();
	SimpleList<LanDevice> STAdevices;
	SimpleList<LanDevice> APdevices;
	
	void setOnDeviceFound(void (*deviceFoundCallback)(LanDevice));
	void setOnDeviceLost(void (*deviceLostCallback)(LanDevice));
	
	private:
	void (*onDeviceFound)(LanDevice);
	void (*onDeviceLost)(LanDevice);
	static void rescan_callback( void * context );
	static void arp_timer_callback( void * context );
	uint16_t refreshInterval;

};
extern LanDevicesClass LanDevices;
  
#endif