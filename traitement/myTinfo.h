// **********************************************************************************
// ESP8266 Traitement teleinfo
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
// Modifi� par marc PRIEUR 2019-05-01 ()
//		-int�gr� le code dans la classe myTinfo myTinfo.cpp
//
//
//********************************************************************************
#ifndef MYTINFO_H
#define MYTINFO_H
#include "LibTeleinfo.h"
//void ADPSCallback(uint8_t phase);
void NewFrame(ValueList * me);
//void UpdatedFrame(ValueList * me);

class myTinfo
{
public:
	myTinfo();
	void init(boolean modeLinkyHistorique);
	bool getTask_emoncms(void);
	bool getTask_jeedom(void);
	bool getTask_httpRequest(void);
	bool getEtResetImax(void);
	bool getEtResetCgtCompteur(void);
	bool getNouvelleTrame(void) const;
	void clrNouvelleTrame(void);
	//chaque seconde : true si  ((WiFi.status() == WL_CONNECTED) && (wifi_station_get_connect_status() == STATION_GOT_IP)) sinon false
	void setEtatWifi(bool etat);
	bool getEtatWifi(void) const;
	uint8_t getEtResetErreur(void);
private:
	//chaque seconde : true si  ((WiFi.status() == WL_CONNECTED) && (wifi_station_get_connect_status() == STATION_GOT_IP)) sinon false
	bool etatWifi;	
	uint8_t erreur = 0;
};
extern myTinfo MYTINFO;
#endif

