#ifndef OTA_h
#define OTA_h

class NiceOTAClass{
	public:
	void begin();

	private:
	static void OTA_callback( void * context );
};

extern NiceOTAClass NiceOTA;
  
#endif