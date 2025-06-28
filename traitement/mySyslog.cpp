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
// Modifie par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//
// Modifie par marc PRIEUR 2019-03-21 ()
//		-integre le code dans la classe mySyslog mySyslog.cpp
//********************************************************************************
#include <Arduino.h>
#include "Wifinfo.h"
#include <FS.h>
#include <WiFiUdp.h>
#include <Syslog.h>   //add Syslog-master to C:\Program Files (x86)\Arduino\libraries
#include "config.h"
#include "constantes.h"
#include "mySyslog.h"

#ifdef SYSLOG
	WiFiUDP udpClient;
	Syslog syslog(udpClient, SYSLOG_PROTO_IETF);
#endif


/////////////////////////// uniquement si DEBUGSERIAL ou SYSLOG //////////
#ifdef MACRO
////// Versions polymorphes des appels au debugging
////// non liees au port Serial ou Serial1

	void mySyslog::convert(const __FlashStringHelper *ifsh)
	{
	  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
	  plog=0;
	  while (1) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) {
			logbuffer[plog]=0;
		  break;
		}
		logbuffer[plog]=c;
		plog++;
	  }
	}



#ifdef SYSLOG


void mySyslog::process_line(char *msg) {
	strcat(waitbuffer, msg);
	pending = strlen(waitbuffer);
	if (waitbuffer[pending - 1] == 0x0D || waitbuffer[pending - 1] == 0x0A) {
		//Cette ligne est complete : l'envoyer !
		for (uint i = 0; i < pending - 1; i++) {
			if (waitbuffer[i] <= 0x20)
				waitbuffer[i] = 0x20;
		}
		syslog.log(LOG_INFO, waitbuffer);
		delay(2 * pending);
		memset(waitbuffer, 0, 255);
		pending = 0;

	}
}
#endif





// Toutes les fonctions aboutissent sur la suivante :
void mySyslog::Myprint(char *msg) {

#ifdef DEBUGSERIAL
	DEBUG_SERIAL.print(msg);
#endif

#ifdef SYSLOG
	//if( SYSLOGusable ) 

	if (WIFIOK)
	{
		process_line(msg);
	}
	else if (SYSLOGselected) {
		//syslog non encore disponible
		//stocker les messages e envoyer plus tard
		in++;
		if (in >= 50) {
			//table saturee !
			in = 0;
		}
		if (lines[in]) {
			//entree occupee : l'ecraser, tant pis !
			free(lines[in]);
		}
		lines[in] = (char *)malloc(strlen(msg) + 2);
		memset(lines[in], 0, strlen(msg + 1));
		strcpy(lines[in], msg);
	}
#endif

}

void mySyslog::Myprint() {
	logbuffer[0] = 0;
	Myprint(logbuffer);
}
void mySyslog::Myprint(String msg) {
	sprintf(logbuffer, "%s", msg.c_str());
	Myprint(logbuffer);
}
void mySyslog::Myprint(const __FlashStringHelper *msg) {
	convert(msg);
	Myprint(logbuffer);
}
void mySyslog::Myprint(long i) {
	sprintf(logbuffer, "%ld", i);
	Myprint(logbuffer);
}
void mySyslog::Myprint(unsigned int i) {
	sprintf(logbuffer, "%d", i);
	Myprint(logbuffer);
}
void mySyslog::Myprint(int i) {
	sprintf(logbuffer, "%d", i);
	Myprint(logbuffer);
}
void mySyslog::Myprintln() {
	sprintf(logbuffer, "\n");
	Myprint(logbuffer);
}
void mySyslog::Myprintln(unsigned char *msg) {
	sprintf(logbuffer, "%s\n", msg);
	Myprint(logbuffer);
}
void mySyslog::Myprintln(String msg) {
	sprintf(logbuffer, "%s\n", msg.c_str());
	Myprint(logbuffer);
}
void mySyslog::Myprintln(const __FlashStringHelper *msg) {
	convert(msg);
	logbuffer[plog] = (char)'\n';
	logbuffer[plog + 1] = 0;
	Myprint(logbuffer);
}
void mySyslog::Myprintln(long i) {
	sprintf((char *)logbuffer, "%ld\n", i);
	Myprint(logbuffer);
}
void mySyslog::Myprintln(unsigned int i) {
	sprintf(logbuffer, "%d\n", i);
	Myprint(logbuffer);
}
void mySyslog::Myprintln(int i) {
	sprintf(logbuffer, "%d\n", i);
	Myprint(logbuffer);
}

void mySyslog::Myflush() {

}

#endif    //MACRO






#ifdef SYSLOG
	void ICACHE_FLASH_ATTR mySyslog::configSyslog(void)
	{
		if (*CONFIGURATION.config.syslog_host) {
//if (MYCONFIGURATION.config.syslog_host != '\0') {
			SYSLOGselected = true;
			// Create a new syslog instance with LOG_KERN facility
			syslog.server(CONFIGURATION.config.syslog_host, CONFIGURATION.config.syslog_port);
			syslog.deviceHostname(CONFIGURATION.config.host);
			syslog.appName(APP_NAME);
			syslog.defaultPriority(LOG_KERN);
			memset(waitbuffer, 0, 255);
			pending = 0;
			//#ifdef START_STOP_WIFI
			//	  wifi = true;
			//#endif
				 // SYSLOGusable=true;
		}
		else {
			// SYSLOGusable=false;
			SYSLOGselected = false;
		}
	}
	void mySyslog::setSYSLOGselected(void)
	{
	SYSLOGselected = true;  //Par defaut, au moins stocker les premiers msg debug
	//  SYSLOGusable=false;   //Tant que non connecte, ne pas emettre sur reseau
	}

	void mySyslog::clearLinesSyslog(void)
	{
	for (int i = 0; i < 50; i++)
		lines[i] = 0;
	in = -1;
	out = -1;
	}

void mySyslog::sendSyslog(void)
{
	if (SYSLOGselected) {
		if (in != out && in != -1) {
			//Il y a des messages en attente d'envoi
			out++;
			while (out <= in) {
				process_line(lines[out]);
				free(lines[out]);
				lines[out] = 0;
				out++;
			}
			DebuglnF("syslog buffer empty");
		}
	}
	else {
		DebuglnF("syslog not activated !");
	}
}
#endif
