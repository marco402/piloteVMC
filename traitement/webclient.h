// **********************************************************************************
// ESP8266 Teleinfo WEB Client, routine include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// Modifie par marc Prieur 2019
//		-V2.0.0:integre le code dans la classe webClient webClient.cpp  webClient.h
//		-V2.0.2:ajout de TAILLEBUFEMONCMS
// Using library ESP8266HTTPClient version 1.1
//
// **********************************************************************************

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <Arduino.h> 
#include <ESP8266HTTPClient.h>
#include "Wifinfo.h"
#include "enregistrement.h"
#include "LibTeleinfo.h" 

#define TAILLEBUFEMONCMS 400

const char FP_QC[] PROGMEM = "\"";
const char FP_QVC[] PROGMEM = ",\"";
const char FP_QCDP[] PROGMEM = "\":";
const char FP_JSON_START[] PROGMEM = "{\r\n";
const char FP_JSON_END[] PROGMEM = "\r\n}\r\n";
const char FP_QCQ[] PROGMEM = "\":\"";
const char FP_QCNL[] PROGMEM = "\",\r\n\"";
const char FP_RESTART[] PROGMEM = "OK, Redemarrage en cours\r\n";
const char FP_NL[] PROGMEM = "\r\n";

class webClient
{
public:
	webClient();
	boolean emoncmsPost(void);
	boolean jeedomPost(void);
	boolean httpRequest(void);
	//boolean UPD_switch(void);
	String  build_emoncms_json(void);
	String build_emoncms_json_vmc(ST_message leMessage);
	boolean emetEmoncmsJsonVmc(ST_message leMessage);
	//void httpGetPost(void);
private:
	boolean httpPost(char * host, uint16_t port, char * url);
	//boolean modeLinkyHistorique;
	//modeTil   mode; // Teleinfo mode (legacy/historique vs standard)
	boolean transfertHttpEnCours = false;
	//HTTPClient http;
	unsigned long attenteHttp = 0;
};
extern webClient WEBCLIENT;
#endif
