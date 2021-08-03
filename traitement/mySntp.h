#ifndef MYSNTP_H
#define MYSNTP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Wifinfo.h" 
#include "SNTPTime.h"
#include "SNTPClock.h"

extern SNTPClock Clock;
void Task_1_Sec(void);
class mySntp
{
public:
	mySntp();
	~mySntp();
	//time_t getSeconds1970() const;
	void init(void);
	boolean getCycle1Seconde(void);
	bool getMinuit(void) const;
	void clrMinuit();
	bool TestSiMinuit(void);
private:
	bool minuit = false;
};


#endif

