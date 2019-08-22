// **********************************************************************************
// ESP8266 Teleinfo WEB Server
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// Modifi� par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//
// Modifi� par marc PRIEUR 2019-03-21 ()
//		-int�gr� le code dans la classe myWifi myWifi.cpp
//
//Using library ESP8266WiFi version 1.0
//Using library ESP8266mDNS version 0.0.0
//
//********************************************************************************
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

#include "Wifinfo.h"
#include "mySyslog.h"
#include "config.h"
#include "ledsRGBSerial.h"
#include "myOTA.h"
#include "mySNTP.h"
#include "myWifi.h"


//#ifdef START_STOP_WIFI
//**************************************WiFiOn**********************************
bool myWifi::WiFiOn() {
	uint8_t timeout;
	timeout = 50;			//>500ms.
	wifi_fpm_do_wakeup();
	wifi_fpm_close();
	//ajout
	wifi_set_opmode(STATION_MODE);
	wifi_station_connect();
	//
	ESP.wdtFeed();
	while (!WIFIOK || timeout>0)
	{
		--timeout;
		yield();
		delay(10);
	}
	if (WIFIOK)
	{
		DebugF("wifi out sleep");
		nb_reconnect++;
		return true;
	}
		DebugF("wifi timeout pb out sleep");
		return false;

}

//**************************************WiFiOff**********************************
void myWifi::WiFiOff() {
	uint8_t timeout;
	wifi_station_disconnect();
	timeout = 50; // > 500ms.
	while ((wifi_station_get_connect_status() != STATION_IDLE) && timeout)
	{
		delay(10);
		--timeout;
	} 
	//ajout
	wifi_set_opmode(NULL_MODE);
	wifi_set_sleep_type(MODEM_SLEEP_T);
	//
	wifi_fpm_open();
}

void myWifi::WIFI_printStatus()
{
	DebugF("wifi_get_opmode()=");Debugln((int)wifi_get_opmode());
	DebugF("wifi_station_get_connect_status()=");Debugln((int)wifi_station_get_connect_status());
	DebugF("WiFi.status()=");Debugln((int)WiFi.status());
}

bool myWifi::getWifi(void)
{
	return wifi;
}

void myWifi::testWifi()
{
	if (cptBoot)   //1 heure de wifi apr�s un d�marrage
	{
		cptBoot--;
		on();
		return;
	}
	if (CONFIGURATION.config.tempo.depart_wifi == CONFIGURATION.config.tempo.fin_wifi)
		on();
	else if (CONFIGURATION.config.tempo.depart_wifi < CONFIGURATION.config.tempo.fin_wifi)		//ex:  depart 8h arret 20h-->wifi de 8h a 20 h
	{
		if ((Clock.getHour() >= CONFIGURATION.config.tempo.depart_wifi) && (Clock.getHour() < CONFIGURATION.config.tempo.fin_wifi))
			on();
		else
			off();
	}
	else                                                                                       //ex:  depart 20h arret 8h-->wifi de 20 h � 8 h
	{
		if ((Clock.getHour() >= CONFIGURATION.config.tempo.depart_wifi) || (Clock.getHour() < CONFIGURATION.config.tempo.fin_wifi))
			on();
		else
			off();
	}
}

void myWifi::on(void)
{
	if (!wifi)
	{
		wifi = WiFiOn();
		delay(1);
	}
}
void myWifi::off(void)
{
	if (wifi)
	{
		WiFiOff();
		wifi = false;
		delay(1);
	}
}



/* ======================================================================
Function: WifiHandleConn
Purpose : Handle Wifi connection / reconnection and OTA updates
Input   : setup true if we're called 1st Time from setup
Output  : state of the wifi status
Comments: -
====================================================================== */
int myWifi::WifiHandleConn(boolean setup = false)
{
	char  toprint[20];
	IPAddress ad;

	if (setup) {
#ifdef DEBUGSERIAL
		DebuglnF("========== WiFi diags start");
		WiFi.printDiag(DEBUG_SERIAL);
		DebuglnF("========== WiFi diags end");
		Debugflush();
#endif

		// no correct SSID
		if (!*CONFIGURATION.config.ssid) {
		//if (MYCONFIGURATION.config.ssid == '\0') {
			DebugF("no Wifi SSID in config, trying to get SDK ones...");

			// Let's see of SDK one is okay
			if (WiFi.SSID() == "") {
				DebuglnF("Not found may be blank chip!");
			}
			else {
				//*config.psk = '\0';
				*CONFIGURATION.config.psk = '\0';

				// Copy SDK SSID
				strcpy(CONFIGURATION.config.ssid, WiFi.SSID().c_str());

				// Copy SDK password if any
				if (WiFi.psk() != "")
					strcpy(CONFIGURATION.config.psk, WiFi.psk().c_str());

				DebuglnF("found one!");

				// save back new config
				CONFIGURATION.saveConfig();
			}
		}
		//config perdue voir filtrage mac et server data
		//strncpy(config.ssid, "ssid", CFG_SSID_SIZE);
		//strncpy(config.psk, "cle", CFG_PSK_SIZE);
		//       MYCONFIGURATION.saveConfig();

		// correct SSID
		if (*CONFIGURATION.config.ssid) {


			DebugF("Connecting to: ");Debug(CONFIGURATION.config.ssid);
			Debugflush();
#ifdef IPSTATIC
			// NETWORK: Static IP details...
			IPAddress ip(192, 168, 1, AdresseStatic::TEMPOVMC);        //dhcp sur box 1.20 � 1.100
			IPAddress gateway(192, 168, 1, 1);
			IPAddress subnet(255, 255, 255, 0);
			WiFi.config(ip, gateway, subnet);
#endif
			// Do wa have a PSK ?
			if (*CONFIGURATION.config.psk) {
				// protected network
			   /* Debug(F(" with key '"));
				Debug(config.psk);
				Debug(F("'..."));
				Debugflush();*/
				WiFi.begin(CFG_DEF_SSID, CFG_DEF_PSK);
				//WiFi.begin(CONFIGURATION.config.ssid, CONFIGURATION.config.psk);
			}
			else {
				// Open network
				Debug(F("unsecure AP"));
				Debugflush();
				WiFi.begin(CONFIGURATION.config.ssid);
			}
		}

		uint8_t timeout;
		timeout = 50; // 50 * 200 ms = 5 sec time out
		// 200 ms loop
	   // while ( ( WiFi.status() != WL_CONNECTED) && timeout )
		while (WIFINOOKET && timeout)
		{
			LESLEDS.ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_WIFI);
			delay(50);
			LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
			delay(150);
			--timeout;
		}


		// connected ? disable AP, client mode only
		if (WiFi.status() == WL_CONNECTED)

		{
			nb_reconnect++;         // increase reconnections count
			DebuglnF("connected!");
			WiFi.mode(WIFI_STA);
			ad = WiFi.localIP();
			sprintf(toprint, "%d.%d.%d.%d", ad[0], ad[1], ad[2], ad[3]);
			DebugF("IP address   : "); Debugln(toprint);
			DebugF("MAC address  : "); Debugln(WiFi.macAddress());
			DebugF("\r\n signal %3i dBm\r\n");Debugln( WiFi.RSSI());
#ifdef SYSLOG
			MYSYSLOG.configSyslog();
#endif

			// not connected ? start AP
		}

#ifdef VALIDEAP
		else {
			char ap_ssid[32];
			DebuglnF("Error!");
			Debugflush();

			// STA+AP Mode without connected to STA, autoconnect will search
			// other frequencies while trying to connect, this is causing issue
			// to AP mode, so disconnect will avoid this

			// Disable auto retry search channel
			WiFi.disconnect();

			// SSID = hostname
			strcpy(ap_ssid, MYCONFIGURATION.config.host);
			DebugF("Switching to AP ");Debugln(ap_ssid);
			Debugflush();

			// protected network
			if (*MYCONFIGURATION.config.ap_psk) {
				DebugF(" with key '");Debug(MYCONFIGURATION.config.ap_psk);DebuglnF("'");
				WiFi.softAP(ap_ssid, MYCONFIGURATION.config.ap_psk);
				// Open network
			}
			else {
				DebuglnF(" with no password");
				WiFi.softAP(ap_ssid);
			}
			WiFi.mode(WIFI_AP_STA);
			WiFi.softAPConfig(IPAddress(192, 168, 5, 1), IPAddress(192, 168, 5, 1), IPAddress(255, 255, 255, 0));
			ad = WiFi.softAPIP();
			sprintf(toprint, "%d.%d.%d.%d", ad[0], ad[1], ad[2], ad[3]);
			DebugF("IP address   : "); Debugln(toprint);

			//DebugF("IP address   : "); Debugln(WiFi.softAPIP());
			DebugF("MAC address  : "); Debugln(WiFi.softAPmacAddress());
		}
#endif

//#ifdef START_STOP_WIFI
		wifi = true;
//#endif

		// Set OTA parameters
		MYOTA.configuration();

	} // if setup

	return WiFi.status();
}

int myWifi::getNb_reconnect() const
{
	return nb_reconnect;

}