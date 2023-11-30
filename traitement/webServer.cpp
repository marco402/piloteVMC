// **********************************************************************************
// ESP8266 Teleinfo WEB Server, route web function
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
// Modifi� par marc Prieur 2019
//		-int�gr� le code dans la classe webClient webServer.cpp  webServer.h
//
//		-V2.0.3 
//				-v�rification du nom des champs transf�r�s dans LibTeleinfo
// Using library ESP8266WebServer version 1.0
//
// **********************************************************************************
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "mywifi.h"
#include "myOTA.h"
#include "capteur.h"
#include "smt160.h"
#include "dht.h"
#include "vmc.h"
#ifdef TA12
#include "tA12.h"
#endif
#include "webclient.h"
#include "interRelais.h"
#include "enregistrement.h"
#ifdef ALARME
#include "alarme.h"
#endif
#include "LibTeleinfo.h"
#include "ledsRGBSerial.h"
#include "constantes.h"
#include "config.h"
#include "mySntp.h"
#include "webServer.h"

//https://projetsdiy.fr/esp8266-web-serveur-partie2-interaction-arduino-interface-html/

webServer::webServer()
{
	uint16_t memoPort = 0;
	on("/vmc.json", [&]() {
		DebuglnF("Serving /vmc.json page...");
		String response = "";
		ESP.wdtFeed();  //Force software watchdog to restart from 0
		getVmcJSONData(response);
		// Just to debug where we are
		//Debug(F("Serving /system page..."));
		send(200, "text/json", response);
		yield();  //Let a chance to other threads to work
	});
	//******************************sysJSONTable****************************************
	on("/system.json", [&]() {
		DebuglnF("Serving /system.json page...");
		String response = "";

		ESP.wdtFeed();  //Force software watchdog to restart from 0
		getSysJSONData(response);

		// Just to debug where we are
		//Debug(F("Serving /system page..."));
		send(200, "text/json", response);
		yield();  //Let a chance to other threads to work
	});
	//*******************************handleRoot***************************************
	on("/", [&]() {
		LESLEDS.ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_BLEU);
		handleFileRead("/");
		LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
	});

	//*******************************handleFormConfig***************************************
	on("/config_form.json", [&]() {
		String response = "";
		int ret=0;
		ESP.wdtFeed();  //Force software watchdog to restart from 0
		memoPort = CONFIGURATION.config.tempo.portEnr;
		LESLEDS.ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_BLEU);

		// We validated config ?
		if (hasArg("save"))
		{
			int itemp;
			float itempF;
			DebuglnF("===== Posted configuration");

			// WifInfo
			strncpy(CONFIGURATION.config.ssid, arg("ssid").c_str(), CFG_SSID_SIZE);
			strncpy(CONFIGURATION.config.psk, arg("psk").c_str(), CFG_PSK_SIZE);
			strncpy(CONFIGURATION.config.host, arg("host").c_str(), CFG_HOSTNAME_SIZE);
			strncpy(CONFIGURATION.config.ap_psk, arg("ap_psk").c_str(), CFG_PSK_SIZE);
			strncpy(CONFIGURATION.config.ota_auth, arg("ota_auth").c_str(), CFG_PSK_SIZE);
			itemp = arg("ota_port").toInt();
			CONFIGURATION.config.ota_port = (itemp >= 0 && itemp <= 65535) ? itemp : DEFAULT_OTA_PORT;

			strncpy(CONFIGURATION.config.syslog_host, arg("syslog_host").c_str(), 64);

			itemp = arg("syslog_port").toInt();
			CONFIGURATION.config.syslog_port = (itemp >= 0 && itemp <= 65535) ? itemp : DEFAULT_SYSLOG_PORT;

			// Emoncms
			strncpy(CONFIGURATION.config.emoncms.host, arg("emon_host").c_str(), CFG_EMON_HOST_SIZE);
			strncpy(CONFIGURATION.config.emoncms.url, arg("emon_url").c_str(), CFG_EMON_URL_SIZE);
			strncpy(CONFIGURATION.config.emoncms.apikey, arg("emon_apikey").c_str(), CFG_EMON_APIKEY_SIZE);
			itemp = arg("emon_node").toInt();
			CONFIGURATION.config.emoncms.node = (itemp >= 0 && itemp <= 255) ? itemp : 0;
			itemp = arg("emon_port").toInt();
			CONFIGURATION.config.emoncms.port = (itemp >= 0 && itemp <= 65535) ? itemp : CFG_EMON_DEFAULT_PORT;
			itemp = arg("emon_freq").toInt();
			if (itemp > 0 && itemp <= 86400) {
				// Emoncms Update if needed
				Tick_emoncms.detach();
				Tick_emoncms.attach(itemp, Task_emoncms);
			}
			else {
				itemp = 0;
			}
			CONFIGURATION.config.emoncms.freq = itemp;

			// jeedom
			strncpy(CONFIGURATION.config.jeedom.host, arg("jdom_host").c_str(), CFG_JDOM_HOST_SIZE);
			strncpy(CONFIGURATION.config.jeedom.url, arg("jdom_url").c_str(), CFG_JDOM_URL_SIZE);
			strncpy(CONFIGURATION.config.jeedom.apikey, arg("jdom_apikey").c_str(), CFG_JDOM_APIKEY_SIZE);
			strncpy(CONFIGURATION.config.jeedom.adco, arg("jdom_adco").c_str(), CFG_JDOM_ADCO_SIZE);
			itemp = arg("jdom_port").toInt();
			CONFIGURATION.config.jeedom.port = (itemp > -1 && itemp < 65536) ? itemp : CFG_JDOM_DEFAULT_PORT;
			itemp = arg("jdom_freq").toInt();
			if (itemp > 0 && itemp < 86401) {
				// Emoncms Update if needed
				Tick_jeedom.detach();
				Tick_jeedom.attach(itemp, Task_jeedom);
			}
			else {
				itemp = 0;
			}
			CONFIGURATION.config.jeedom.freq = itemp;

			// HTTP Request
			strncpy(CONFIGURATION.config.httpReq.host, arg("httpreq_host").c_str(), CFG_HTTPREQ_HOST_SIZE);
			strncpy(CONFIGURATION.config.httpReq.path, arg("httpreq_path").c_str(), CFG_HTTPREQ_PATH_SIZE);
			itemp = arg("httpreq_port").toInt();
			CONFIGURATION.config.httpReq.port = (itemp > -1 && itemp < 65536) ? itemp : CFG_HTTPREQ_DEFAULT_PORT;

			itemp = arg("httpreq_freq").toInt();
			if (itemp > 0 && itemp < 86401)
			{
				Tick_httpRequest.detach();
				Tick_httpRequest.attach(itemp, Task_httpRequest);
			}
			else {
				itemp = 0;
			}
			CONFIGURATION.config.httpReq.freq = itemp;

			itemp = arg("httpreq_swidx").toInt();
			if (itemp > 0 && itemp < 65536)
				CONFIGURATION.config.httpReq.swidx = itemp;
			else
				CONFIGURATION.config.httpReq.swidx = 0;
//********************************partie vmc*******************************************
			itempF = arg(CFG_FORM_TEMPO_CORRECTION_TEMP_EXT_ENDIXIEMES).toFloat();
			CONFIGURATION.config.tempo.cor_temp_ext_dixieme_degres = (int8_t)(itempF > -11.0f && itempF < 11.0f) ? itempF *10.0f : CFG_DEF_CORRECTION_TEMP_EXT_PAR10;

			itempF = arg(CFG_FORM_TEMPO_CORRECTION_T_CUISINE_ENDIXIEMES).toFloat();
			CONFIGURATION.config.tempo.cor_temp_cuis_dixieme_degres = (int8_t)(itempF > -11.0f && itempF < 11.0f) ? itempF * 10.0f : CFG_DEF_CORRECTION_T_CUISINE_PAR10;
			itempF = arg(CFG_FORM_TEMPO_CORRECTION_T_SDB_ENDIXIEMES).toFloat();
			CONFIGURATION.config.tempo.cor_temp_sdb_dixieme_degres = (int8_t)(itempF > -11.0f && itempF < 11.0f) ? itempF * 10.0f : CFG_DEF_CORRECTION_T_SDB_PAR10;
			itemp = arg(CFG_FORM_TEMPO_CORRECTION_H_CUISINE).toInt();
			CONFIGURATION.config.tempo.cor_hum_cuis_pourcent = (int8_t)(itemp > -51 && itemp < 51) ? itemp : CFG_DEF_CORRECTION_H_CUISINE;
			itemp = arg(CFG_FORM_TEMPO_CORRECTION_H_SDB).toInt();
			CONFIGURATION.config.tempo.cor_hum_sdb_pourcent = (int8_t)(itemp > -51 && itemp < 51) ? itemp : CFG_DEF_CORRECTION_H_SDB;
			itemp = arg(CFG_FORM_TEMPO_PERIODE_VMC).toInt();
			CONFIGURATION.config.tempo.periode_vmc_sec = (uint16_t)(itemp > 59 && itemp < 1000) ? itemp : CFG_DEF_PERIODE_VMC;
			itemp = arg(CFG_FORM_TEMPO_DUREE_FORCAGE).toInt();
			CONFIGURATION.config.tempo.duree_forcage_sec = (uint16_t)(itemp > 0 && itemp < 100) ? itemp *60: CFG_DEF_DUREE_FORCAGE_SEC;
			itemp = arg(CFG_FORM_TEMPO_DUREE_MINI).toInt();
			CONFIGURATION.config.tempo.duree_mini_sec = (uint16_t)(itemp > -1 && itemp < 300) ? itemp*60 : CFG_DEF_DUREE_MINI_SEC;

			itemp = arg(CFG_FORM_TEMPO_SEUILTEMPERATURECHAUDENDIXIEMES).toInt();
			CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres = (uint16_t)(itemp > -1 && itempF < 100) ? itemp * 10 : CFG_DEF_SEUILTEMPERATURECHAUDENDIXIEMES;
			itemp = arg(CFG_FORM_TEMPO_POURCENT_HUMIDITE).toInt();
			CONFIGURATION.config.tempo.pourcent_hum = (uint8_t)(itemp > 0 && itemp < 21) ? itemp : CFG_DEF_POURCENT_HUMIDITE;
			itemp = arg(CFG_FORM_TEMPO_SEUILTEMPERATUREFROIDENDIXIEMES).toInt();
			CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres = (uint16_t)(itemp > -1 && itemp < 100) ? itemp * 10 : CFG_DEF_SEUILTEMPERATUREFROIDENDIXIEMES;

			itemp = arg(CFG_FORM_TEMPO_DEPART_WIFI).toInt();
			CONFIGURATION.config.tempo.depart_wifi = (uint8_t)(itemp > -1 && itemp < 25) ? itemp : CFG_DEF_DEFART_WIFI;
			itemp = arg(CFG_FORM_TEMPO_PERIODE_ENR).toInt();
			CONFIGURATION.config.tempo.periode_enr = (itemp > 0 && itemp < 86401) ? itemp : CFG_DEF_PERIODE_ENR;
			//itemp = hasArg("present");
			//CONFIGURATION.config.tempo.present = (ETAT_JOUR)(itemp ? ETAT_JOUR::ETAT_JOUR_ABSENT : ETAT_JOUR::ETAT_JOUR_PRESENT);
			itemp = arg(CFG_FORM_TEMPO_FIN_WIFI).toInt();
			CONFIGURATION.config.tempo.fin_wifi = (uint8_t)(itemp > -1 && itemp < 25) ? itemp : CFG_DEF_FIN_WIFI;

			itemp = arg(CFG_FORM_TEMPO_LUMINOSITE_LEDS_RGB).toInt();
			CONFIGURATION.config.tempo.luminositeeLedsRgb = (uint8_t)(itemp >= 0 && itemp <100) ? itemp : CFG_DEF_LUM_LED_RGB;


			itemp = arg(CFG_FORM_TEMPO_PORT_ENR).toInt();
			CONFIGURATION.config.tempo.portEnr = (uint16_t)(itemp > -1 && itemp < 65536) ? itemp : CFG_DEF_PORT_ENR;
			//attention pas de controle sur les hosts
			strncpy(CONFIGURATION.config.tempo.host_enr, arg(CFG_FORM_TEMPO_HOST_ENR).c_str(), CFG_TEMPO_HOST_SIZE);
			//itemp = hasArg("arret_wifi");
			//DebugF("arret_wifi"); Debugln("itemp");
			CONFIGURATION.config.tempo.arret_wifi = hasArg(CFG_FORM_TEMPO_ARRET_WIFI);    // itemp ? !itemp : itemp;
			//CONFIGURATION.config.tempo.acquitementEnr = hasArg(CFG_FORM_TEMPO_ACQUITEMENT_ENR);
			if (CONFIGURATION.saveConfig()) {
				ret = 200;
				response = "OK";
			}
			else {
				ret = 412;
				response = "Unable to save configuration";
			}
			TEMPEXT.setCorrection(CONFIGURATION.config.tempo.cor_temp_ext_dixieme_degres);
			DHTCUISINE_T.setCorrection(CONFIGURATION.config.tempo.cor_temp_cuis_dixieme_degres);
			DHTCUISINE_H.setCorrection(CONFIGURATION.config.tempo.cor_hum_cuis_pourcent);
			DHTSDB.setCorrectionT(CONFIGURATION.config.tempo.cor_temp_sdb_dixieme_degres);
			DHTSDB.setCorrectionH(CONFIGURATION.config.tempo.cor_hum_sdb_pourcent);
			if (memoPort!= CONFIGURATION.config.tempo.portEnr)
			{
			ENREGISTREMENT.stop();
			ENREGISTREMENT.init();
#ifdef ALARME      
			MYALARMEGARAGE.stop();
			MYALARMEGARAGE.init(INDICEALARMES::GARAGE);
			MYALARMEPORTAIL.stop();
			MYALARMEPORTAIL.init(INDICEALARMES::PORTAIL);
#endif
			}
			CONFIGURATION.showConfig();
		}
		else
		{
			ret = 400;
			response = "Missing Form Field";
		}

		//DebugF("Sending response ");Debug(ret);DebugF(":");Debugln(response);
		send(ret, "text/plain", response);

		LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
	});
	//****************************sendJSON******************************************
	on("/json", [&]() {
		boolean first_item = true;
		ValueList * me = TINFO.getList();
		String response = "";

		ESP.wdtFeed();  //Force software watchdog to restart from 0

		DebuglnF("Serving /json page...");
		// Got at least one ?
		if (me) {
			// Json start
			response += FPSTR(FP_JSON_START);
			response += F("\"DATE\":");
			response += Clock.getTimeSeconds();

			// Loop thru the node
			while (me->next) {
				if (!first_item)
					// go to next node
					me = me->next;

				if (!me->free) {
					if (first_item)
						first_item = false;
						response += F(",\"");
						response += me->name;
						response += F("\":");
						formatNumberJSON(response, me->value);
				} //free entry
			} //while
		   // Json end
			response += FPSTR(FP_JSON_END);
		}
		else {
			send(404, "text/plain", "No data");
		}
		send(200, "text/json", response);
		yield();  //Let a chance to other threads to work
	});
	//******************************tinfoJSONTable****************************************
	on("/tinfo.json", [&]() {
		// we're there
		ESP.wdtFeed();  //Force software wadchog to restart from 0
		ValueList * me = TINFO.getList();
		String response = "";

		// Just to debug where we are
		Debugln(F("Serving /tinfo page...\r\n")); 

		if (!me) //&& first_info_call) 
		{
			//Let tinfo such time to build a list....
			DebugF("No list yet...\r\n");
			//first_info_call=false;
			unsigned long topdebut = millis();
			bool expired = false;
			while (!expired) {
				if ((millis() - topdebut) >= 3000) {
					expired = true;   // 3 seconds delay expired
				}
				else {
					yield();  //Let CPU to other threads
				}
			}
			// continue, hoping list values is now ready
			me = TINFO.getList();
		}
		//TINFO.valuesDump(); 
		// Got at least one ?
		if (me) {
			uint8_t index = 0;
			DebuglnF("tinfoJSONTable me...\r\n");
			//first_info_call=false;
			boolean first_item = true;
			// Json start
			response += F("[\r\n");

			// Loop thru the node
			while (me->next) {
				index++;

				if (!first_item)
					// go to next node
					me = me->next;
				else
					if (me->free) {
						//1st item is free : empty list !
						Debugln("Teleinfo list is empty !");
						break;
					}

				if (!me->free) {
					// First item do not add , separator
					if (first_item)
						first_item = false;
					else
						response += F(",\r\n");
						response += F("{\"na\":\"");
						response += me->name;
						response += F("\", \"va\":\"");
//						if (strncmp(me->name, "BBR", 3) == 0) //6 cpt de watt a kw
//						{
//							double x = 0;
//							int y = 0;
//							memcpy( &x,me->value, sizeof(double));
//							y = x / 1000.0;
//							memcpy( me->value,&y ,sizeof(int));
//char  x[15];
//strcpy_P(x, me->value);
//int y =x.toFloat()/ 1000.0;
//memcpy( me->value,&y ,sizeof(int));
//						}
						response += me->value;
						response += F("\", \"ck\":\"");
						if (me->checksum == '"' || me->checksum == '\\' || me->checksum == '/')
							response += '\\';
						response += (char)me->checksum;
						response += F("\", \"fl\":");
						response += me->flags;
						response += '}';
					}
			}
			// Json end
			response += F("\r\n]");
		}
		else {
			Debugln(F("sending 404..."));
			send(404, "text/plain", "No data");
		}
		//Debug(F("sending..."));
		send(200, "text/json", response);
		//Debugln(response);
		//Debugln(F("OK!"));
		yield();  //Let a chance to other threads to work
	});
	//******************************emoncmsJSONTable****************************************
	on("/emoncms.json", [&]() {
		DebuglnF("Serving /emoncms.json page...");
		String response = WEBCLIENT.build_emoncms_json();

		send(200, "text/json", response);
		//Debugln(response);
		//Debugln(F("Ok!"));
		yield();  //Let a chance to other threads to work
	});

	//*******************************confJSONTable***************************************
	on("/config.json", [&]() {
		String response = "";
		//ESP.wdtFeed();  //Force software watchdog to restart from 0
		getConfJSONData(response);
		// Just to debug where we are
		DebuglnF("Serving /config page...");
		send(200, "text/json", response);
		Debugln(F("getConfJSONData Ok!"));
		yield();  //Let a chance to other threads to work
	});
	//*******************************spiffsJSONTable***************************************
	on("/spiffs.json", [&]() {
		String response = "";
		//ESP.wdtFeed();  //Force software watchdog to restart from 0
		getSpiffsJSONData(response);
		send(200, "text/json", response);
		yield();  //Let a chance to other threads to work
	});
	//*********************************wifiScanJSON*************************************
	on("/wifiscan.json", [&]() {
		String response = "";
		bool first = true;

		// Just to debug where we are
		DebuglnF("Serving /wifiscan page...");

		int n = WiFi.scanNetworks();

		// Json start
		response += F("[\r\n");

		for (uint8_t i = 0; i < n; ++i)
		{
			int8_t rssi = WiFi.RSSI(i);

			//uint8_t percent = 0;

			//// dBm to Quality
			//if (rssi <= -100)
			//	percent = 0;
			//else if (rssi >= -50)
			//	percent = 100;
			//else
			//	percent = 2 * (rssi + 100);

			if (first)
				first = false;
			else
				response += F(",");

			response += F("{\"ssid\":\"");
			response += WiFi.SSID(i);
			response += F("\",\"rssi\":");
			response += rssi;
			response += FPSTR(FP_JSON_END);
		}

		// Json end
		response += FPSTR("]\r\n");

		DebugF("sending...");
		send(200, "text/json", response);
		DebuglnF("Ok!");
		yield();  //Let a chance to other threads to work
	});
	//*****************************handleFactoryReset*****************************************
	on("/factory_reset", [&]() {
		// Just to debug where we are
		DebuglnF("Serving /factory_reset page...");
		CONFIGURATION.ResetConfig();
		ESP.eraseConfig();
		DebuglnF("sending...");
		send(200, "text/plain", FPSTR(FP_RESTART));
		DebuglnF("ResetConfig Ok!");
		delay(1000);
		ESP.restart();
		while (true)
			delay(1);
	});
	//****************************handleReset******************************************
	on("/reset", [&]() {
		// Just to debug where we are
		DebuglnF("Serving /reset page...");
		DebuglnF("sending...");
		send(200, "text/plain", FPSTR(FP_RESTART));
		DebuglnF("Ok!");
		delay(1000);
		ESP.restart();
		while (true)
			delay(1);
	});
	//****************************heartbeat******************************************
	on("/hb.htm", HTTP_GET, [&]() {
		sendHeader("Connection", "close");
		sendHeader("Access-Control-Allow-Origin", "*");
		send(200, "text/html", R"(OK)");
	});
	//******************************update****************************************
	//In general, POST should be used for requests that potentially modify state on the server, and GET should be used for read - only operations
	on("/update", HTTP_POST,
		// handler once file upload finishes
		[&]() {
		sendHeader("Connection", "close");
		sendHeader("Access-Control-Allow-Origin", "*");
		send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		ESP.restart();
	},
		// handler for upload, get's the sketch bytes, 
		// and writes them through the Update object
		[&]() {
		HTTPUpload& upload = this->upload();
		char  buffer[132];
		if (upload.status == UPLOAD_FILE_START) {
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			DebugF("ESP.getFreeSketchSpace() = ");Debugln(ESP.getFreeSketchSpace());
			DebugF("maxSketchSpace = ");Debugln(maxSketchSpace);

			WiFiUDP::stopAll();
			
			sprintf(buffer, "Update: %s\n", upload.filename.c_str());
			Debug(buffer);



			MYOTA.setOta_blink();

			//ajout pour spiffs
			int command = U_FLASH;

//#ifdef ESP8266
//command = (filename.indexOf("spiffs") > -1) ? U_FS : U_FLASH;
//if(command =U_FS)
//  DebuglnF("command = U_FS");
//else
//  DebuglnF("command = U_FLASH");  
//#else
//command = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
//#endif

//comment when spiffs to littlefs      
//			if (strstr(upload.filename.c_str(), "spiffs"))
//			{
//				command = U_SPIFFS;
//				DebuglnF("command = U_SPIFFS");
//			}
//			else
//				DebuglnF("command = U_FLASH");



			if (!Update.begin(maxSketchSpace, command))
			{
				Update.printError(Serial);			//Serial1
				DebugF("Erreur maxSketchSpace = ");	Debugln(maxSketchSpace);
			}

		}
		else if (upload.status == UPLOAD_FILE_WRITE) {

			if (MYOTA.getOta_blink()) {
				LESLEDS.ledRGBON(LES_LEDS_EN_SERIE::LED_BWRJOUR, COULEURS::C_SPIFF);
			}
			else {
				LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRJOUR);
			}
			MYOTA.notOta_blink();
			//ota_blink = !ota_blink;

			DebugF(".");
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
				Update.printError(Serial);			//Serial1

		}
		else if (upload.status == UPLOAD_FILE_END) {
			//true to set the size to the current progress
			if (Update.end(true)) {
				sprintf(buffer, "Update Success: %u\nRebooting...\n", upload.totalSize);
				Debug(buffer);
			}
			else
				Update.printError(Serial);			//Serial1

			LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRJOUR);

		}
		else if (upload.status == UPLOAD_FILE_ABORTED) {
			DebuglnF("avant Update.end()");
			Update.end();

			LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRJOUR);

			DebuglnF("Update was aborted");
		}
		delay(0);
	});
	//****************************lecture d'un enregistrement******************************************
	on("/getEnr.json",[&]() {
				//ESP.wdtFeed();  //Force software watchdog to restart from 0
		String response = "";
		ST_message leMessage=ENREGISTREMENT.getEnregistrementPourJson();
		if (leMessage.heure1970>0)
		{
		response = WEBCLIENT.build_emoncms_json_vmc(leMessage);
		// Just to debug where we are
		send(200, "text/json", response);
		}
		//yield();  //Let a chance to other threads to work
	});
	//****************************acquitement enregistrement******************************************
	on("/acqEnr",  [&]() {
		sendHeader("Connection", "close");
		sendHeader("Access-Control-Allow-Origin", "*");
		send(200, "text/html", R"(OK)");
		ENREGISTREMENT.setdernierMessageEmisOK();
	});
	//****************************handleNotFound******************************************
	onNotFound([&]() {
		String response = "";
		boolean found = false;
		char  buffer[132];

		LESLEDS.ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_BLEU);

		// try to return SPIFFS file
		found = handleFileRead(uri());

		// Try Teleinfo ETIQUETTE
		if (!found) {
			// We check for an known label
			ValueList * me = TINFO.getList();
			const char * uri;
			// convert uri to char * for compare
			uri = this->uri().c_str();

			sprintf(buffer, "handleNotFound(%s)\r\n", uri);
			Debug(buffer);

			// Got at least one and consistent URI ?
			if (me && uri && *uri == '/' && *++uri) {

				// Loop thru the linked list of values
				while (me->next && !found) {

					// go to next node
					me = me->next;

					//Debugf("compare to '%s' ", me->name);
					// Do we have this one ?
					if (strcmp(me->name, uri) == 0)
					{
						// no need to continue
						found = true;

						// Add to respone
						response += F("{\"");
						response += me->name;
						response += F("\":");
						formatNumberJSON(response, me->value);
						response += F("}\r\n");
					}
				}
			}

			// Got it, send json
			if (found)
				send(200, "text/json", response);
		}

		// All trys failed
		if (!found) {
			// send error message in plain text
			String message = F("File Not Found\n\n");
			message += F("URI: ");
			message += uri();
			message += F("\nMethod: ");
			message += (method() == HTTP_GET) ? "GET" : "POST";
			message += F("\nArguments: ");
			message += args();
			message += FPSTR(FP_NL);

			for (uint8_t i = 0; i < args(); i++) {
				message += " " + argName(i) + ": " + arg(i) + FPSTR(FP_NL);
			}

			send(404, "text/plain", message);
		}
		LESLEDS.ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
	});
}	//FIN DU CONSTRUCTEUR

void webServer::initServeur(void)	//myServer::
{

	initOptVal();

// serves all SPIFFS Web file with 24hr max-age control
// to avoid multiple requests to ESP
#ifdef LITTLE_FS
	serveStatic("/font", LittleFS, "/font", "max-age=86400");
	serveStatic("/js", LittleFS, "/js", "max-age=86400");
	serveStatic("/css", LittleFS, "/css", "max-age=86400");
#else
  serveStatic("/font", SPIFFS, "/font", "max-age=86400");
  serveStatic("/js", SPIFFS, "/js", "max-age=86400");
  serveStatic("/css", SPIFFS, "/css", "max-age=86400");
#endif
begin(80);	//

}
void webServer::initSpiffs(void)	//
{
	char  buffer[132];
//*****************************************SPIFFS****************************************
#ifdef LITTLE_FS
// Init LittleFS filesystem, to use web server static files
	if (!LittleFS.begin())
	{
		// Serious problem
		DebuglnF("LittleFS Mount failed !");
	}
	else
	{
		DebuglnF("");
		DebuglnF("LittleFS Mount succesfull");

		Dir dir = LittleFS.openDir("/");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			sprintf(buffer, "FS File: %s, size: %d\n", fileName.c_str(), fileSize);
			Debug(buffer);
		}
	}
#else
  // Init SPIFFS filesystem, to use web server static files
  if (!SPIFFS.begin())
  {
	  // Serious problem
	  DebuglnF("SPIFFS Mount failed !");
  }
  else
  {
	  DebuglnF("");
	  DebuglnF("SPIFFS Mount succesfull");

	  Dir dir = SPIFFS.openDir("/");
	  while (dir.next()) {
		  String fileName = dir.fileName();
		  size_t fileSize = dir.fileSize();
		  sprintf(buffer, "FS File: %s, size: %d\n", fileName.c_str(), fileSize);
		  Debug(buffer);
	  }
  }
#endif
  //*****************************************fin SPIFFS****************************************
}

/* ======================================================================
Function: handleFileRead
Purpose : return content of a file stored on SPIFFS file system
Input   : file path
Output  : true if file found and sent
Comments: -
====================================================================== */
bool webServer::handleFileRead(String path) {
	if (path.endsWith("/"))
		path += "index.htm";

	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";

	DebugF("handleFileRead ");Debug(path);
#ifdef LITTLE_FS
	if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
		if (LittleFS.exists(pathWithGz)) {
			path += ".gz";
			DebugF(".gz");
		}

		DebuglnF(" found on LittleFS");

		File file = LittleFS.open(path, "r");
#else
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz)) {
			path += ".gz";
			DebugF(".gz");
		}

		DebuglnF(" found on SPIFFS");

		File file = SPIFFS.open(path, "r");
#endif
		//size_t sent = 
		streamFile(file, contentType);
		file.close();
		return true;
	}

	DebuglnF("");

	send(404, "text/plain", "File Not Found");
	return false;
}


/* ======================================================================
Function: formatNumberJSON
Purpose : check if data value is full number and send correct JSON format
Input   : String where to add response
		  char * value to check
Output  : -
Comments: 00150 => 150
		  ADCO  => "ADCO"
		  1     => 1
====================================================================== */
void webServer::formatNumberJSON(String &response, char * value)
{
	// we have at least something ?
	if (value && strlen(value))
	{
		boolean isNumber = true;
		//    uint8_t c;
		char * p = value;

		// just to be sure
		if (strlen(p) <= 16) {
			// check if value is number
			while (*p && isNumber) {
				if (*p < '0' || *p > '9')
					isNumber = false;
				p++;
			}

			// this will add "" on not number values
			if (!isNumber) {
				response += '\"';
				response += value;
				response += F("\"");
			}
			else {
				// this will remove leading zero on numbers
				p = value;
				while (*p == '0' && *(p + 1))
					p++;
				response += p;
			}
		}
		else {
			Debugln(F("formatNumberJSON error!"));
		}
	}
}




/* ======================================================================
Function: formatSize
Purpose : format a asize to human readable format
Input   : size
Output  : formated string
Comments: -
====================================================================== */
String webServer::formatSize(size_t bytes)
{
	if (bytes < 1024) {
		return String(bytes) + F(" Byte");
	}
	else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + F(" KB");
	}
	else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + F(" MB");
	}
	else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + F(" GB");
	}
}

/* ======================================================================
Function: getContentType
Purpose : return correct mime content type depending on file extension
Input   : -
Output  : Mime content type
Comments: -
====================================================================== */
String webServer::getContentType(String filename) {
	if (filename.endsWith(".htm")) return F("text/html");
	else if (filename.endsWith(".html")) return F("text/html");
	else if (filename.endsWith(".css")) return F("text/css");
	else if (filename.endsWith(".json")) return F("text/json");
	else if (filename.endsWith(".js")) return F("application/javascript");
	else if (filename.endsWith(".png")) return F("image/png");
	else if (filename.endsWith(".gif")) return F("image/gif");
	else if (filename.endsWith(".jpg")) return F("image/jpeg");
	else if (filename.endsWith(".ico")) return F("image/x-icon");
	else if (filename.endsWith(".xml")) return F("text/xml");
	else if (filename.endsWith(".pdf")) return F("application/x-pdf");
	else if (filename.endsWith(".zip")) return F("application/x-zip");
	else if (filename.endsWith(".gz")) return F("application/x-gzip");
	else if (filename.endsWith(".otf")) return F("application/x-font-opentype");
	else if (filename.endsWith(".eot")) return F("application/vnd.ms-fontobject");
	else if (filename.endsWith(".svg")) return F("image/svg+xml");
	else if (filename.endsWith(".woff")) return F("application/x-font-woff");
	else if (filename.endsWith(".woff2")) return F("application/x-font-woff2");
	else if (filename.endsWith(".ttf")) return F("application/x-font-ttf");
	return "text/plain";
}
/* ======================================================================
Function: getSysJSONData
Purpose : Return JSON string containing system data
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void webServer::getVmcJSONData(String & response)
{
	response = "";
	//char buffer[64];
	//int32_t adc =  (1000 * analogRead(A0) / 1024);

	 // Json start
	response += F("[\r\n");
	//TempCuis
	response += "{\"na\":\"Temp cuis\",\"va\":\"";
	response += DHTCUISINE_T.getMesureCycleOled();
	response += "\"},\r\n";
	//Temp sdb
	response += "{\"na\":\"Temp sdb\",\"va\":\"";
	response += DHTSDB.DHT_T.getMesureCycleOled();
	response += "\"},\r\n";
	//Temp ext
	response += "{\"na\":\"Temp ext\",\"va\":\"";
	response += TEMPEXT.getMesureCycleOled();
	response += "\"},\r\n";
	//Hum cuis
	response += "{\"na\":\"Hum cuis\",\"va\":\"";
	response += DHTCUISINE_H.getMesureCycleOled();
	response += "\"},\r\n";
	//Hum sdb
	response += "{\"na\":\"Hum sdb\",\"va\":\"";
	response += DHTSDB.DHT_H.getMesureCycleOled();
	response += "\"},\r\n";
	//I vmc
	response += "{\"na\":\"I vmc\",\"va\":\"";
	response += "0";   // TA12.getMesureCycleOled();
	response += "\"},\r\n";
	//mode
	response += "{\"na\":\"Mode\",\"va\":\"";
	response += VMC.getLeModeString();
	response += "\"},\r\n";
	//etat
	response += "{\"na\":\"Etat\",\"va\":\"";
	response += RELAIS.getEtatReelRelaisMarcheArretString();
	response += "\"},\r\n";
	//vitesse
	response += "{\"na\":\"Vitesse\",\"va\":\"";
	response +=RELAIS.getEtatRelaisVitesseString();
	response += "\"},\r\n";
	//nb minutes du jour
	response += "{\"na\":\"Nb min vmc\",\"va\":\"";
	response += RELAIS.getNbMinuteActiveJourCourant();
	response += "\"},\r\n";
	//d�compte mode forc�
	response += "{\"na\":\"Decompte\",\"va\":\"";
	response += VMC.getDecompteTempoArretMarcheForce();
	response += "\"},\r\n";
	//seuil chaud
	response += "{\"na\":\"Seuil chaud\",\"va\":\"";
	response += (CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres/10);
	response += "\"},\r\n";
	//seuil froid
	response += "{\"na\":\"Seuil froid\",\"va\":\"";
	response += (CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres/10);
	response += "\"},\r\n";
	//seuil hum cuis
	response += "{\"na\":\"Seuil hum cuis\",\"va\":\"";
	response += VMC.getSeuilHC();
	response += "\"},\r\n";
	//seuil hum sdb
	response += "{\"na\":\"Seuil hum sdb\",\"va\":\"";
	response += VMC.getSeuilHB();
	response += "\"},\r\n";
	// enregistrement
	response += "{\"na\":\"Nb d'enregistrement\",\"va\":\"";			//3
	response += ENREGISTREMENT.getComptMessage();
	response += "/";
	response += NBMESSAGE;
	response += "ptE:";
	response += ENREGISTREMENT.getPtre();
	response += "ptL:";
	response += ENREGISTREMENT.getPtrl();
	response += "\"}\r\n"; // Last don't have comma at end

	// Json end
	response += F("]\r\n");
}


/* ======================================================================
Function: getSysJSONData
Purpose : Return JSON string containing system data
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void webServer::getSysJSONData(String & response)
{
	response = "";
	char buffer[64];
	//int32_t adc =  (1000 * analogRead(A0) / 1024);

	 // Json start
	response += F("[\r\n");

	response += "{\"na\":\"Uptime\",\"va\":\"";
	response += Clock.getDateTimeStr();       //NTP.sysinfo.sys_uptime;
	response += "\"},\r\n";
	//tm t = *Clock.getTimeStruct();
	//sprintf_P(buffer,"t %i-%02i-%02i %02i:%02i:%02i\r\n",	t.tm_year, t.tm_mon + MONTH_START, t.tm_mday,t.tm_hour, t.tm_min, t.tm_sec);
	//Debugln(buffer);
#ifdef SENSOR
	response += "{\"na\":\"Switch\",\"va\":\"";
	if (SwitchState)
		response += F("Open");  //switch ouvert
	else
		response += F("Closed");  //switch ferm�

	response += "\"},\r\n";
#endif

	if (WiFi.status() == WL_CONNECTED)
	{
		response += "{\"na\":\"Wifi RSSI\",\"va\":\"";
		response += WiFi.RSSI();
		response += " dB\"},\r\n";
		response += "{\"na\":\"Wifi network\",\"va\":\"";
		response += CONFIGURATION.config.ssid;
		response += "\"},\r\n";
		uint8_t mac[] = { 0, 0, 0, 0, 0, 0 };
		uint8_t* macread = WiFi.macAddress(mac);
		char macaddress[20];
		sprintf_P(macaddress, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), macread[0], macread[1], macread[2], macread[3], macread[4], macread[5]);
		response += "{\"na\":\"Adresse MAC station\",\"va\":\"";
		response += macaddress;
		response += "\"},\r\n";
	}
	response += "{\"na\":\"Nb reconnexions Wifi\",\"va\":\"";
	response += WIFI.getNb_reconnect();
	response += "\"},\r\n";

	response += "{\"na\":\"Alterations Data detectees\",\"va\":\"";
	response += nb_reinit;
	response += "\"},\r\n";

	response += "{\"na\":\"WifInfo Version\",\"va\":\"" WIFINFO_VERSION "\"},\r\n";

	response += "{\"na\":\"Compile le\",\"va\":\"" __DATE__ " " __TIME__ "\"},\r\n";

	response += "{\"na\":\"Options de compilation\",\"va\":\"";
	response += optval;
	response += "\"},\r\n";

	response += "{\"na\":\"SDK Version\",\"va\":\"";
	response += system_get_sdk_version();
	response += "\"},\r\n";

	response += "{\"na\":\"Chip ID\",\"va\":\"";
	sprintf_P(buffer, "0x%0X", system_get_chip_id());
	response += buffer;
	response += "\"},\r\n";

	response += "{\"na\":\"Boot Version\",\"va\":\"";
	sprintf_P(buffer, "0x%0X", system_get_boot_version());
	response += buffer;
	response += "\"},\r\n";

	response += "{\"na\":\"Flash Real Size\",\"va\":\"";
	response += formatSize(ESP.getFlashChipRealSize());
	response += "\"},\r\n";

	response += "{\"na\":\"Firmware Size\",\"va\":\"";
	response += formatSize(ESP.getSketchSize());
	response += "\"},\r\n";

	response += "{\"na\":\"Free Size\",\"va\":\"";
	response += formatSize(ESP.getFreeSketchSpace());
	response += "\"},\r\n";

	response += "{\"na\":\"Analog\",\"va\":\"";
	//adc = ((1000 * analogRead(A0)) / 1024);
	//adc = ESP.getVcc();  //pas juste du au pont 220/100k  d'apr�s internet......
	sprintf_P(buffer, PSTR("%d mV"), (1000 * analogRead(A0) / 1024));
	response += buffer;
	response += "\"},\r\n";

	FSInfo info;
#ifdef LITTLE_FS
	LittleFS.info(info);
#else
	SPIFFS.info(info);
#endif

	response += "{\"na\":\"SPIFFS Total\",\"va\":\"";
	response += formatSize(info.totalBytes);
	response += "\"},\r\n";

	response += "{\"na\":\"SPIFFS Used\",\"va\":\"";
	response += formatSize(info.usedBytes);
	response += "\"},\r\n";

	response += "{\"na\":\"SPIFFS Occupation\",\"va\":\"";
	sprintf_P(buffer, "%d%%", 100 * info.usedBytes / info.totalBytes);
	response += buffer;
	response += "\"},\r\n";

	// Free mem should be last one 
	response += "{\"na\":\"Free Ram\",\"va\":\"";
	response += formatSize(system_get_free_heap_size());
	response += "\"}\r\n"; // Last don't have comma at end

	// Json end
	response += F("]\r\n");
}

/* ======================================================================
Function: getConfigJSONData
Purpose : Return JSON string containing configuration data
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void webServer::getConfJSONData(String & r)
{
	// Json start
	r = FPSTR(FP_JSON_START);

	r += "\"";
	r += CFG_FORM_SSID;      r += FPSTR(FP_QCQ); r += CONFIGURATION.config.ssid;           r += FPSTR(FP_QCNL);
	//r += CFG_FORM_PSK;       r += FPSTR(FP_QCQ); r += CONFIGURATION.config.psk;            r += FPSTR(FP_QCNL);
	r += CFG_FORM_PSK;       r += FPSTR(FP_QCQ); r += "************";            r += FPSTR(FP_QCNL);
	r += CFG_FORM_HOST;      r += FPSTR(FP_QCQ); r += CONFIGURATION.config.host;           r += FPSTR(FP_QCNL);
	r += CFG_FORM_AP_PSK;    r += FPSTR(FP_QCQ); r += CONFIGURATION.config.ap_psk;         r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_HOST; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.host;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_PORT; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.port;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_URL;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.url;    r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_KEY;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.apikey; r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_NODE; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.node;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_EMON_FREQ; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.emoncms.freq;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_OTA_AUTH;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.ota_auth;       r += FPSTR(FP_QCNL);
	r += CFG_FORM_OTA_PORT;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.ota_port;       r += FPSTR(FP_QCNL);
	r += CFG_FORM_SYSLOG_HOST; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.syslog_host;  r += FPSTR(FP_QCNL);
	r += CFG_FORM_SYSLOG_PORT; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.syslog_port;  r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_HOST; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.host;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_PORT; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.port;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_URL;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.url;    r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_KEY;  r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.apikey; r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_ADCO; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.adco;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_JDOM_FREQ; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.jeedom.freq;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_HTTPREQ_HOST; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.httpReq.host;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_HTTPREQ_PORT; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.httpReq.port;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_HTTPREQ_PATH; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.httpReq.path;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_HTTPREQ_FREQ; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.httpReq.freq;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_HTTPREQ_SWIDX; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.httpReq.swidx;

	r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_CORRECTION_H_CUISINE; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.cor_hum_cuis_pourcent;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_CORRECTION_H_SDB; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.cor_hum_sdb_pourcent;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_CORRECTION_TEMP_EXT_ENDIXIEMES; r += FPSTR(FP_QCQ); r += (CONFIGURATION.config.tempo.cor_temp_ext_dixieme_degres/10.0f);   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_CORRECTION_T_CUISINE_ENDIXIEMES; r += FPSTR(FP_QCQ); r +=( CONFIGURATION.config.tempo.cor_temp_cuis_dixieme_degres / 10.0f);   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_CORRECTION_T_SDB_ENDIXIEMES; r += FPSTR(FP_QCQ); r += (CONFIGURATION.config.tempo.cor_temp_sdb_dixieme_degres / 10.0f);   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_DUREE_FORCAGE; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.duree_forcage_sec/60;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_DUREE_MINI; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.duree_mini_sec/60;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_PERIODE_VMC; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.periode_vmc_sec;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_POURCENT_HUMIDITE; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.pourcent_hum;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_SEUILTEMPERATUREFROIDENDIXIEMES; r += FPSTR(FP_QCQ); r += (CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres / 10.0f);   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_SEUILTEMPERATURECHAUDENDIXIEMES; r += FPSTR(FP_QCQ); r += (CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres / 10.0f);   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_DEPART_WIFI; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.depart_wifi;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_FIN_WIFI; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.fin_wifi;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_PERIODE_ENR; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.periode_enr;   r += FPSTR(FP_QCNL);
	//int present = CONFIGURATION.config.tempo.present == ETAT_JOUR_PRESENT ? 0 : 1;
	//r += CFG_FORM_TEMPO_PRESENT; r += FPSTR(FP_QCQ); r += present;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_HOST_ENR; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.host_enr;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_PORT_ENR; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.portEnr;   r += FPSTR(FP_QCNL);
	r += CFG_FORM_TEMPO_ARRET_WIFI; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.arret_wifi;   r += FPSTR(FP_QCNL);
	//r += CFG_FORM_TEMPO_ACQUITEMENT_ENR; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.acquitementEnr;
	r += CFG_FORM_TEMPO_LUMINOSITE_LEDS_RGB; r += FPSTR(FP_QCQ); r += CONFIGURATION.config.tempo.luminositeeLedsRgb;

	r += F("\"");
	// Json end
	r += FPSTR(FP_JSON_END);

}
bool first_item = true;
#ifdef LITTLE_FS
void webServer::getDirLittleFSJSONData(String  Path,String & response)
{
	Dir dir = LittleFS.openDir(Path);
	while (dir.next())
	{
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();


		if (dir.fileSize()) {      //no directory
			if (first_item)
				first_item = false;
			else
				response += ",";
			response += F("{\"na\":\"");
			response += Path + fileName.c_str();
			response += F("\",\"va\":\"");
			response += fileSize;
			response += F("\"}\r\n");
		}
	}
}
#endif
/* ======================================================================
Function: getSpiffsJSONData
Purpose : Return JSON string containing list of SPIFFS files
Input   : Response String
Output  : -
Comments: -
====================================================================== */
void webServer::getSpiffsJSONData(String & response)
{
	//  char buffer[32];
	bool first_item = true;

	// Json start
	response = FPSTR(FP_JSON_START);

	// Files Array  
	response += F("\"files\":[\r\n");

	// Loop trough all files
#ifdef LITTLE_FS
    getDirLittleFSJSONData("/",response);
    getDirLittleFSJSONData("/css/",response);
    getDirLittleFSJSONData("/fonts/",response);
    getDirLittleFSJSONData("/js/",response);
#else
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		if (first_item)
			first_item = false;
		else
			response += ",";

		response += F("{\"na\":\"");
		response += fileName.c_str();
		response += F("\",\"va\":\"");
		response += fileSize;
		response += F("\"}\r\n");
	}
#endif
	response += F("],\r\n");


	// SPIFFS File system array
	response += F("\"spiffs\":[\r\n{");

	// Get SPIFFS File system informations
	FSInfo info;
#ifdef LITTLE_FS
	LittleFS.info(info);
#else
	SPIFFS.info(info);
#endif
	response += F("\"Total\":");
	response += info.totalBytes;
	response += F(", \"Used\":");
	response += info.usedBytes;
	response += F(", \"ram\":");
	response += system_get_free_heap_size();
	response += F("}\r\n]");

	// Json end
	response += FPSTR(FP_JSON_END);
}

void webServer::initOptVal(void)
{

	//********************************Options de compilation pour page html***************************************  
	memset(optval, 0, 100);

#ifdef DEBUGSERIAL
	strcat(optval, "DEBUGSERIAL, "); //13
#else
	strcat(optval, ", ");
#endif

#ifdef SYSLOG
	strcat(optval, "SYSLOG, "); //8
#else
	strcat(optval, ", ");
#endif

#ifdef SIMUTRAMETEMPO
	strcat(optval, "SIMUTRA, ");	//9
#else
	strcat(optval, ", ");
#endif

#ifdef COMP_CAN_BUS
	strcat(optval, "COMP_CAN_BUS, ");	//5
#else
	strcat(optval, ", ");
#endif
#ifdef AVEC_NTP
	strcat(optval, "NTP, ");	//5
#else
	strcat(optval, ", ");
#endif
#ifdef MODE_HISTORIQUE
	strcat(optval, "MODE_HISTO, ");	//12
#else
	strcat(optval, "MODE_STANDARD,");
#endif
#ifdef TELEINFO_RXD2
	strcat(optval, "Tinfo RXD2");	//10
#else
	strcat(optval, "Tinfo RXD0");
#endif
	//********************************fin Options de compilation pour page html*************************************** 
	Debugln(F("=============="));
	DebugF("WifInfo V");DebuglnF(WIFINFO_VERSION);
	DebugF("Options : ");Debugln(optval);
	Debugln();
}


void webServer::incNb_reinit(void)
{
	nb_reinit++;
}
