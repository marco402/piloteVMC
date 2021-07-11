//paramétrage arduino
//type de carte NodeMcu 0.9
//cpu frequency 160Mhz
//flash size 4M(1M spiffs)
//rename to traitement_tempo_vmc.ino.bin  
//C:\Users\mireille\AppData\Local\Temp\arduino_build_135849\traitement.ino.bin
//10/07/2021->seuil air frais 25->20    pour ventiler plus la nuit en été

//10/07/2021->20,5,23,-0.5,-1.8,0,-17,20,1,15,300,300,0,24,192.168.1.69,8889,10
//10/07/2021->blanc,OTA_AUTH,8266,192.168.1.69,514

//https://github.com/esp8266/Arduino
// **********************************************************************************
// ESP8266 Teleinfo WEB Server
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
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
// Modifié par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//       Version 1.0.5
//       Librairie LibTeleInfo : Allocation statique d'un tableau de stockage 
//           des variables (50 entrées) afin de proscrire les malloc/free
//           pour éviter les altérations des noms & valeurs
//       Modification en conséquence des séquences de scanning du tableau
//       ATTENTION : Nécessite probablement un ESP-8266 type Wemos D1,
//        car les variables globales occupent 42.284 octets
//
//       Version 1.0.5a (11/01/2018)
//       Permettre la mise à jour OTA à partir de fichiers .ino.bin (Auduino IDE 1.8.3)
//       Ajout de la gestion d'un switch (Contact sec) relié à GND et D5 (GPIO-14)
//          Décommenter le #define SENSOR dans Wifinfo.h
//          Pour être utilisable avec Domoticz, au moins l'URL du serveur et le port
//          doivent être renseignés dans la configuration HTTP Request, ainsi que 
//          l'index du switch (déclaré dans Domoticz)
//          L'état du switch (On/Off) est envoyé à Domoticz au boot, et à chaque
//            changement d'état
//       Note : Nécessité de flasher le SPIFFS pour pouvoir configurer l'IDX du switch
//              et flasher le sketch winfinfo.ino.bin via interface Web
//       Rendre possible la compilation si define SENSOR en commentaire
//              et DEFINE_DEBUG en commentaire (aucun debug, version Production...)
//
//       Version 1.0.6 (04/02/2018) Branche 'syslog' du github
//		      Ajout de la fonctionnalité 'Remote Syslog'
//		        Pour utiliser un serveur du réseau comme collecteur des messages Debug
//            Note : Nécessité de flasher le SPIFFS pour pouvoir configurer le remote syslog
//          Affichage des options de compilation sélectionnées dans l'onglet 'Système'
//            et au début du Debug + syslog éventuels
// **********************************************************************************
// Modifié par marc PRIEUR 2019-03-21 V2.0.0
//		V2.0.2:2019/05/01 ajout de la classe myTinfo
//###################################includes##################################  
//version wifinfo syslog d'origine:352 392 bytes
#include "constantes.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "Wifinfo.h"
#include "mySNTP.h"
#include "mySyslog.h"
#include "myOTA.h"
#include "capteur.h"
#include "smt160.h"
#include "dht.h"
#include "vmc.h"
#include "tA12.h"
#include "canBus.h"
#include "simuTempo.h"
#include "LibTeleinfo.h"
#include "enregistrement.h"
#include "interRelais.h"
#include "ledsRGBSerial.h"
#include "webServer.h"
#include "webclient.h"
#include "config.h"
#include "myWifi.h"
#include "buzzer.h"
#include "myTinfo.h"
//###################################defines################################## 
myTinfo MYTINFO;
#ifdef COMP_CAN_BUS
	can_bus CAN_BUS;
#endif
enregistrement ENREGISTREMENT;
relais RELAIS;
 lesLeds LESLEDS;   //conserver pour TRAITEMENTLEDS même sans led locale
#ifdef SIMUTRAMETEMPO
	SimuTempo SIMU_TEMPO;
#endif 
configuration CONFIGURATION;
//les capteurs
smt160 TEMPEXT;
capteur CAPTEURIINST;
capteur  DHTCUISINE_T;
capteur DHTCUISINE_H;
dht DHTSDB;
ta12 TA12;
Ticker Tick_emoncms;
Ticker Tick_jeedom;
Ticker Tick_httpRequest;
mySntp MYSNTP;  // AVEC_NTP filtre que les appels NTP mais garder l'objet ntp pour l'heure de départ à 0
TInfo TINFO;
myOTA MYOTA;
webServer WEBSERVER;
#ifdef SYSLOG
mySyslog MYSYSLOG;
#endif
myWifi WIFI;
webClient WEBCLIENT(MODE_HISTORIQUE);
vmc VMC;
buzzer BUZZER;
//###################################SETUP##################################  
/* ======================================================================
Function: setup
Purpose : Setup I/O and other one time startup stuff
Input   : -
Output  : - 
Comments: -
====================================================================== */
void ICACHE_FLASH_ATTR setup() {

  system_update_cpu_freq(160);

#ifdef SYSLOG
  MYSYSLOG.setSYSLOGselected();
  MYSYSLOG.clearLinesSyslog();
#endif

#ifdef DEBUGSERIAL
  DEBUG_SERIAL.begin(115200);		//pour test recup SPI
  DEBUG_SERIAL.setDebugOutput(true);
  while (!Serial) {
	  ; // wait for serial port to connect. Needed for native USB port only
  }
#endif
#ifdef PIN_LED_RGB
  LESLEDS.initLed();
#endif
  CONFIGURATION.initConfig();
  WIFI.WifiHandleConn(true);
#ifdef SYSLOG
  MYSYSLOG.sendSyslog();
#endif
 CONFIGURATION.showConfig();
  //initialiser les capteurs après CONFIGURATION.initConfig() 
  TEMPEXT.initCapteur(PIN_CAPTEUR_TEMP_EXT, CONFIGURATION.config.tempo.cor_temp_ext_dixieme_degres, 10, ID_CLASSES::ID_TEMPEXT);
  CAPTEURIINST.initCapteur(0, 1, ID_CLASSES::ID_TA12);
  DHTCUISINE_T.initCapteur(CONFIGURATION.config.tempo.cor_temp_cuis_dixieme_degres, 10, ID_CLASSES::ID_DHTCUISINE_T);
  DHTCUISINE_H.initCapteur(CONFIGURATION.config.tempo.cor_hum_cuis_pourcent, 1, ID_CLASSES::ID_DHTCUISINE_H);
  DHTSDB.initCapteur(PIN_CAPTEUR_TEMP_HUMIDITE_SDB, ID_CLASSES::ID_DHTSDB);
  TA12.initCapteur(PIN_CAPTEUR_ANA_COURANT, 1);
  TA12.setSeuilPetiteVitesse(20);
  TA12.setSeuilGrandeVitesse(39);
  DHTSDB.DHT_H.initCapteur( CONFIGURATION.config.tempo.cor_hum_sdb_pourcent, 1, ID_CLASSES::ID_DHTSDB_H);
  DHTSDB.DHT_T.initCapteur(CONFIGURATION.config.tempo.cor_temp_sdb_dixieme_degres, 10, ID_CLASSES::ID_DHTSDB_T);
  WEBSERVER.initSpiffs();  
  WEBSERVER.initServeur();
#ifdef  TELEINFO_RXD2 
  DebuglnF("Fin des traces consoles voir avec syslog,la teleinfo est recue sur RXD2");
  DebuglnF("Changement de vitesse des traces consoles voir avec syslog,la teleinfo est maintenant recu sur RXD0 a la place de l'USB(OU RXD2 si Serial.swap())");
#else
	#if MODE_HISTORIQUE
		DebuglnF("Les traces consoles continue de fonctionner sur TXD0 à 1200 bauds ou VITESSE_SIMUTRAMETEMPO si SIMUTRAMETEMPO");
		DebuglnF("La teleinfo est recue sur RXD0");
	#else
		DebuglnF("Les traces consoles continue de fonctionner sur TXD0 à 9600 bauds ou VITESSE_SIMUTRAMETEMPO si SIMUTRAMETEMPO");
	#endif
#endif
#if MODE_HISTORIQUE
	#ifdef SIMUTRAMETEMPO
		#ifdef TELEINFO_RXD2
			DebuglnF("Pour la simulation:strapper D4(TXD1) et D7(RXD2)");
		#else
			DebuglnF("Pour la simulation:strapper D4(TXD1) et D9(RXD0");
		#endif
		const int VITESSE_SIMUTRAMETEMPO = 115200;
		Serial.begin(VITESSE_SIMUTRAMETEMPO);
		Serial.setRxBufferSize(1024);
	#else
		Serial.begin(1200, SERIAL_7E1);
		Serial.setDebugOutput(true);
		DebuglnF("Serial.begin");
	#endif
#else
	Serial.begin(9600, SERIAL_7E1);       //5.3.5. Couche physique document enedis Enedis-NOI-CPT_54E.pdf 
#endif
#ifdef TELEINFO_RXD2
  Serial.swap();  // reception teleinfo sur rxd2 sinon passe la reception teleinfo sur rx0 pour recuperer rx2 pour mosi
					  //fonctionne correctement sur rx0, il faut juste penser à enlever le strap de la simulation si present
					  //pour programmer ou debuguer via la console, pas de pb en OTA.
#endif
#ifdef SIMUTRAMETEMPO
	SIMU_TEMPO.initSimuTrameTempo();
	SerialSimu.begin(VITESSE_SIMUTRAMETEMPO);	//19200, SERIAL_7E1
#endif
MYTINFO.init(MODE_HISTORIQUE);
  uint8_t timeout = 5;
  while (WIFINOOKOU && timeout)
  {
	  delay(500);
	  DebugF(".");
	  --timeout;
  }
MYSNTP.init();
ENREGISTREMENT.init();
#ifdef COMP_CAN_BUS
  CAN_BUS.InitCanBus(MCP_16MHZ);
#endif
}
//###################################LOOP##################################  
/* ======================================================================
Function: loop
Purpose : infinite loop main code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void loop()
{
	unsigned long start = 0;
	unsigned long duree = 0;
	static uint16_t dureeMax = 0; 
  start = millis();

//**************************************Traitement des entrées****************************************************
  if (MYTINFO.getEtResetImax())
  {
  char  valeurs[] = { "\0" };
  CAPTEURIINST.traiteMaxi(atoi(TINFO.valueGet(&TableauTempoName[TEMPO_UTILISE::ETU_IINST][0], &valeurs[0])));
  }
#ifdef COMP_CAN_BUS
  CAN_BUS.traiteReception();
#endif
//************************************************1 fois par seconde*************************************************************************************
  // Only once task per loop, let system do its own task
 if (MYSNTP.getCycle1Seconde()) {
	WEBSERVER.handleClient();
	MYOTA.handle();
	//**************************************Traitement enregistrement****************************************************
	MYSNTP.TestSiMinuit();
	boolean CgtCompteur = MYTINFO.getEtResetCgtCompteur();
	if(MYSNTP.getMinuit() && CgtCompteur)
	{
		ENREGISTREMENT.clrPremierEnregistrement();	//envoi des 6 compteurs
		RELAIS.razNbSecondeActiveJourCourant();
		MYSNTP.clrMinuit();
	}
	/*ENREGISTREMENT.testAcquitementMessage();*/
	ENREGISTREMENT.TRAITEENREGISTREMENT(MYTINFO.getNouvelleTrame(), CgtCompteur,dureeMax);
	//DebugF(" dureeMax: "); Debugln(dureeMax);
	dureeMax = 0;
	//**************************************Traitement des capteurs****************************************************
	//lecture du courant ici pour supprimer l'effet de la commande des relais
	//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
	VMC.traiteRazCapteursAuto();
	//  circuit a revoir entrée hs   TA12.traitementLectureTa12();   //retour:false->buzzer->true pour buzzer
	//DebugF("beep:TA12 "); Debugln(retour);
	//**************************************Traitement des relais VMC****************************************************
	VMC.TRAITEMENTVMC();
	//DebugF("tCuis: "); Debugln(tCuis);
//**************************************Traitement des sorties****************************************************
#ifdef COMP_CAN_BUS
	CAN_BUS.TRAITEMENTEMISSIONCAN();
#endif 	
	if (WIFIOK)
	{
#ifdef EMISSION_ENREGISTREMENT
		ENREGISTREMENT.traitementEmissionMessageTempoVMC();     //passe un cycle pour acquitement de la commande,émission de 1 à 10 enregistrement en mémoire
#endif
		MYTINFO.setEtatWifi(true); 
	}
	else
		MYTINFO.setEtatWifi(false);
	//WIFI.testWifi();			//test debut et fin wifi supprimé,si le retour a lieu pendant la coupure horaire de la box, le programme n'est plus traité...solution???
#ifdef SIMUTRAMETEMPO
	SIMU_TEMPO.traite1Trame(Clock.getTimeSeconds());                      //(NTP.getSeconds1970());
#endif
   LESLEDS.TRAITEMENTLEDS();
   VMC.incrementeCptVmc();
   MYTINFO.clrNouvelleTrame();
  }  //getCycle1Seconde
  //*************************************************************************************************************************************
	else if (MYTINFO.getTask_emoncms())
      WEBCLIENT.emoncmsPost(); 
	else if (MYTINFO.getTask_jeedom())
	  WEBCLIENT.jeedomPost();
	else if (MYTINFO.getTask_httpRequest())
	  WEBCLIENT.httpRequest();
 	if (TINFO.getReinit())
	{
    //Some polluted entries have been detected in Teleinfo ListValues
		//need_reinit=false;
		WEBSERVER.incNb_reinit();    //account of reinit operations, for system infos
		MYTINFO.init(MODE_HISTORIQUE);//Clear ListValues, buffer, and wait for next STX
	} 
	else
	{
	  if ( Serial.available() )
			{ 
				char c = Serial.read();          //pour test recup SPI
				//Debug(c);   ATTENTION plante l'uc  plus d'OTA...
				TINFO.process(c);
		}
	}
	unsigned long t = millis();
	duree =t - start;
	if (duree > dureeMax)
		dureeMax = (int)duree;
 }   //loop
