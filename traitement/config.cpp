// **********************************************************************************
// ESP8266 Teleinfo WEB Server configuration
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
//Using library EEPROM version 1.0
// 
// **********************************************************************************

#define noRESETCONFIG

#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include <EEPROM.h>
#include "constantes.h"
#include "interRelais.h" 
#include "config.h" 
void ICACHE_FLASH_ATTR configuration::initConfig(void) //
{
// Our configuration is stored into EEPROM
	EEPROM.begin(sizeof(_Config));
	// Clear our global flags
	config.config = 0;

#ifdef RESETCONFIG
  ResetConfig();
#endif  
	// Read Configuration from EEP
  //start comment for reset config
	///*
  
	if (readConfig()) {
		DebuglnF("Good CRC, not set! From now, we can use EEPROM config !");
	}
	else {
  //*/
  //end comment
		// Reset Configuration
		ResetConfig();
		// save back
		//saveConfig();   deja dans ResetConfig
		// Indicate the error in global flags
		config.config |= CFG_BAD_CRC;
		//DebuglnF("Reset to default");
   //start comment
   ///*
	}
 //*/
 //end comment
}
/* ======================================================================
Function: ResetConfig
Purpose : Set configuration to default values
Input   : -
Output  : -
Comments: -
====================================================================== */
void ICACHE_FLASH_ATTR configuration::ResetConfig(void)
{
	// Start cleaning all that stuff
	memset(&config, 0, sizeof(_Config));
	strcpy_P(config.ssid, PSTR(CFG_DEF_SSID));
	strcpy_P(config.psk, PSTR(CFG_DEF_PSK));
	// Set default Hostname
	sprintf_P(config.host, PSTR("WifInfo-%06X"), ESP.getChipId());
	strcpy_P(config.ota_auth, PSTR(DEFAULT_OTA_AUTH));
	config.ota_port = DEFAULT_OTA_PORT;
	// Add other init default config here
	// Emoncms
	strcpy_P(config.emoncms.host, CFG_EMON_DEFAULT_HOST);
	config.emoncms.port = CFG_EMON_DEFAULT_PORT;
	strcpy_P(config.emoncms.url, CFG_EMON_DEFAULT_URL);
	strcpy_P(config.emoncms.apikey, CFG_EMON_DEFAULT_APIKEY);
	config.emoncms.node = CFG_EMON_DEFAULT_NODE_ID;
	// Jeedom
	strcpy_P(config.jeedom.host, CFG_JDOM_DEFAULT_HOST);
	config.jeedom.port = CFG_JDOM_DEFAULT_PORT;
	strcpy_P(config.jeedom.url, CFG_JDOM_DEFAULT_URL);
	//strcpy_P(config.jeedom.adco, CFG_JDOM_DEFAULT_ADCO);
	// HTTP Request
	strcpy_P(config.httpReq.host, CFG_HTTPREQ_DEFAULT_HOST);
	config.httpReq.port = CFG_HTTPREQ_DEFAULT_PORT;
	strcpy_P(config.httpReq.path, CFG_HTTPREQ_DEFAULT_PATH);
	config.tempo.cor_hum_cuis_pourcent = CFG_DEF_CORRECTION_H_CUISINE;
	config.tempo.cor_hum_sdb_pourcent = CFG_DEF_CORRECTION_H_SDB;
	config.tempo.cor_temp_cuis_dixieme_degres = CFG_DEF_CORRECTION_T_CUISINE_PAR10;
	config.tempo.cor_temp_ext_dixieme_degres = CFG_DEF_CORRECTION_TEMP_EXT_PAR10;
	config.tempo.cor_temp_sdb_dixieme_degres = CFG_DEF_CORRECTION_T_SDB_PAR10;
	config.tempo.pourcent_hum = CFG_DEF_POURCENT_HUMIDITE;
	config.tempo.seuil_temp_chaud_dixieme_degres = CFG_DEF_SEUILTEMPERATURECHAUDENDIXIEMES;
	config.tempo.duree_forcage_sec = CFG_DEF_DUREE_FORCAGE_SEC;
	config.tempo.duree_mini_sec = CFG_DEF_DUREE_MINI_SEC;
	config.tempo.seuil_temp_froid_dixieme_degres = CFG_DEF_SEUILTEMPERATUREFROIDENDIXIEMES;
	config.tempo.periode_vmc_sec = CFG_DEF_PERIODE_VMC;
	config.tempo.arret_wifi = CFG_DEF_ARRET_WIFI;
	config.tempo.depart_wifi = CFG_DEF_DEFART_WIFI;
	config.tempo.fin_wifi = CFG_DEF_FIN_WIFI;
	config.tempo.periode_enr = CFG_DEF_PERIODE_ENR;
	config.tempo.portEnr = CFG_DEF_PORT_ENR;
	config.tempo.luminositeeLedsRgb = CFG_DEF_LUM_LED_RGB;
	//config.tempo.acquitementEnr = CFG_DEF_ACQUITEMENT_ENR;
	config.syslog_port= DEFAULT_SYSLOG_PORT;
	strcpy_P(config.syslog_host, CFG_DEF_HOST_ENR);
	// config.tempo.host_enr = CFG_DEF_HOST_ENR;
	strncpy(config.tempo.host_enr, CFG_DEF_HOST_ENR, CFG_TEMPO_HOST_SIZE);
	config.config |= CFG_RGB_LED;
	// save back
	saveConfig();
}
uint16_t ICACHE_FLASH_ATTR configuration::crc16Update(uint16_t crc, uint8_t a)
{
  int i;
  crc ^= a;
  for (i = 0; i < 8; ++i)  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}
/* ======================================================================
Function: eeprom_dump
Purpose : dump eeprom value to serial 
Input 	: -
Output	: -
Comments: -
====================================================================== */
void ICACHE_FLASH_ATTR configuration::eepromDump(uint8_t bytesPerRow)
{
	uint16_t i;  // , b;
  char buf[10];
  uint16_t j=0 ;
  
  // default to 16 bytes per row
  if (bytesPerRow==0)
    bytesPerRow=16;
  Debugln();
  // loop thru EEP address
  for (i = 0; i < sizeof(_Config); i++) {
    // First byte of the row ?
    if (j==0) {
			// Display Address
      sprintf(buf,"%04X : ", i);
//      Debug(buf);
    }
    // write byte in hex form
    sprintf(buf,"%02X ", EEPROM.read(i));
    Debug(buf);
		// Last byte of the row ?
    // start a new line
    if (++j >= bytesPerRow) {
			j=0;
//			Debugln();
		}
  }
}

/* ======================================================================
Function: readConfig
Purpose : fill config structure with data located into eeprom
Input 	: true if we need to clear actual struc in case of error
Output	: true if config found and crc ok, false otherwise
Comments: -
====================================================================== */
bool ICACHE_FLASH_ATTR configuration::readConfig (bool clear_on_error)
{
	uint16_t crc = ~0;
	uint8_t * pconfig = (uint8_t *) &config ;
	uint8_t data ;
	// For whole size of config structure
	for (uint16_t i = 0; i < sizeof(_Config); ++i) {
		// read data
		data = EEPROM.read(i);
		// save into struct
		*pconfig++ = data ;
		// calc CRC
		crc = crc16Update(crc, data);
	}
	// CRC Error ?
	if (crc != 0) {
		// Clear config if wanted
    if (clear_on_error)
		  memset(&config, 0, sizeof( _Config ));
		return false;
	}
	return true ;
}

/* ======================================================================
Function: saveConfig
Purpose : save config structure values into eeprom
Input 	: -
Output	: true if saved and readback ok
Comments: once saved, config is read again to check the CRC
====================================================================== */
bool ICACHE_FLASH_ATTR configuration::saveConfig (void)
{
  uint8_t * pconfig ;
  bool ret_code;
  //eepromDump(32);
  // Init pointer 
  pconfig = (uint8_t *) &config ;
	// Init CRC
  config.crc = ~0;
  // For whole size of config structure, pre-calculate CRC
  for (uint16_t i = 0; i < sizeof (_Config) - 2; ++i)
	  config.crc = crc16Update(config.crc, *pconfig++);
	// Re init pointer 
  pconfig = (uint8_t *) &config ;
  // For whole size of config structure, write to EEP
  for (uint16_t i = 0; i < sizeof(_Config); ++i)
  {
    EEPROM.write(i, *pconfig++);
 }
  // Physically save
  EEPROM.commit();
 
  // Read Again to see if saved ok, but do 
  // not clear if error this avoid clearing
  // default config and breaks OTA
  ret_code = readConfig(false);
   DebugF("Write config ");
#ifdef DEBUGSERIAL
  if (ret_code)
    Debugln(F("OK!"));
  else
    Debugln(F("Error!"));
#endif
  return (ret_code);
}

/* ======================================================================
Function: showConfig
Purpose : display configuration
Input 	: -
Output	: -
Comments: -
====================================================================== */
void ICACHE_FLASH_ATTR configuration::showConfig()
{
  DebugF("Config size="); Debug(sizeof(_Config));
  DebugF(" (emoncms=");   Debug(sizeof(_emoncms));
  DebugF("  jeedom=");   Debug(sizeof(_jeedom));
  DebugF("  http request=");   Debug(sizeof(_httpRequest));
  DebugF("  tempo=");   Debug(sizeof(_tempo));
  Debugln(" )");
  DebuglnF("===== Wifi"); 
  DebugF("ssid     :"); Debugln(config.ssid);
#ifdef SYSLOG
  //Debugln(config.psk);   // DebugF("psk      :***********"); sinon remplacé par des * au prochain changement de config
#endif
#ifdef DEBUGSERIAL
  DEBUG_SERIAL.print("psk      :");
  DEBUG_SERIAL.println(config.psk);
#endif
  DebugF("host     :"); Debugln(config.host);
  DebuglnF("===== Avancé"); 
  DebugF("ap_psk   :"); Debugln(config.ap_psk);
  DebugF("OTA auth :"); Debugln(config.ota_auth);
  DebugF("OTA port :"); Debugln(config.ota_port);
  DebugF("syslog host :"); Debugln(config.syslog_host);
  DebugF("syslog port :"); Debugln(config.syslog_port);
  DebugF("Config   :"); 
  if (config.config & CFG_RGB_LED) DebugF(" RGB");
  if (config.config & CFG_DEBUG)   DebugF(" DEBUG");
  if (config.config & CFG_LCD)     DebugF(" LCD");
  Debugln("");
 
  DebuglnF("===== Emoncms"); 
  DebugF("host     :"); Debugln(config.emoncms.host);
  DebugF("port     :"); Debugln((int)config.emoncms.port);
  DebugF("url      :"); Debugln(config.emoncms.url);
  DebugF("key      :"); Debugln("*****************");      //Debugln(config.emoncms.apikey);
  DebugF("node     :"); Debugln(config.emoncms.node);
  DebugF("freq     :"); Debugln(config.emoncms.freq);

  DebuglnF("===== Jeedom"); 
  DebugF("host     :"); Debugln(config.jeedom.host);
  DebugF("port     :"); Debugln(config.jeedom.port);
  DebugF("url      :"); Debugln(config.jeedom.url);
  DebugF("key      :"); Debugln(config.jeedom.apikey);
  DebugF("compteur :"); Debugln(config.jeedom.adco);
  DebugF("freq     :"); Debugln(config.jeedom.freq);

  DebuglnF("===== HTTP request"); 
  DebugF("host     :"); Debugln(config.httpReq.host);
  DebugF("port     :"); Debugln(config.httpReq.port);
  DebugF("path     :"); Debugln(config.httpReq.path);
  DebugF("freq     :"); Debugln(config.httpReq.freq);
  DebugF("sw idx   :"); Debugln(config.httpReq.swidx);

  DebugF("cor hum cuis      :"); Debugln(config.tempo.cor_hum_cuis_pourcent);
  DebugF("cor hum sdb       :"); Debugln(config.tempo.cor_hum_sdb_pourcent);
  DebugF("cor temp cuis     :"); Debugln(config.tempo.cor_temp_cuis_dixieme_degres);
  DebugF("cor temp ext      :"); Debugln(config.tempo.cor_temp_ext_dixieme_degres);
  DebugF("cor temp sdb      :"); Debugln(config.tempo.cor_temp_sdb_dixieme_degres);
  DebugF("pourcent hum      :"); Debugln(config.tempo.pourcent_hum);
  DebugF("seuil temp froid  :"); Debugln(config.tempo.seuil_temp_froid_dixieme_degres);
  DebugF("duree forcage     :"); Debugln(config.tempo.duree_forcage_sec);
  DebugF("duree mini     :"); Debugln(config.tempo.duree_mini_sec);
  DebugF("seuil temp chaud  :"); Debugln(config.tempo.seuil_temp_chaud_dixieme_degres);		//seuil->froid;delta->chaud
  DebugF("periode vmc       :"); Debugln(config.tempo.periode_vmc_sec);
  DebugF("periode_enr       :"); Debugln(config.tempo.periode_enr);
  DebugF("depart_wifi       :"); Debugln(config.tempo.depart_wifi);
  DebugF("fin_wifi          :"); Debugln(config.tempo.fin_wifi);
  DebugF("portEnr           :"); Debugln(config.tempo.portEnr);
  DebugF("host enr          :"); Debugln(config.tempo.host_enr);
  DebugF("arret wifi        :"); Debugln(config.tempo.arret_wifi);
  DebugF("luminosite led rgb:"); Debugln(config.tempo.luminositeeLedsRgb);
  //DebugF("acquitement enr   :"); Debugln(config.tempo.acquitementEnr);

  //DebugF("present           :"); Debugln(config.tempo.present);

  delay(1000);
}
