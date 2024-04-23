//20/04/2024
//blocage affichage ajout alternance :,sans : sur l'horloge 7 segments.
//ajout indicateur CgtCompteur pour afficher l'heure du changement en bas de l'afficheur.

//2/11/23
//blocage affichage
//cote traitement else()a chaque cycle
//reinit environ toutes 53 secondes
//compteur sur page web 206...
//reinit affichage->OK

//21/10/23
//rafraichissement affichage toutes les 12/15secondes
//reinit affichage idem
//pas de message syslog
//site pilote vmc ok  temps rafraichi toutes les 2 secondes
//reception enregistrements ok



//13/10/23
//blocage canBus cote affichage
//message debug cote traitement:
//192.168.1.78  Oct 13 08:02:36 1 kern  info  -  Wifinfo - - - ﻿nbReinitCanBusReception 275 
//192.168.1.78  Oct 13 08:02:37 1 kern  info    -  Wifinfo - - - ?CAN BUS Shield init (speed SPI:500k,speed CAN:250k,freq CAN 16Mhz) ok! 
//reinit que affichage--->ok

//a coder:le 18/10/23 ajout messages debug
//9/2023 blocage liaison vers affichage dans les 2 sens
//-redemarrage affichage pareil
//-pas de message syslog server
//-nombre d'erreur détectée = 3


//12/11/23 arduino 1.8.19
//a la compilation sant aucune mise a jour:
//xtensa-lx106-elf-g++: error: unrecognized debug output level '++'  ???   reglage du probleme:arduino/menu/fichier/preference
//avertissement du compilateur rien pb,tout ok,rien ok.



//11/2022 erreur changement d'heure:
//changement dimanche à 2 heures du matin
//lun. oct. 31 17:05:27 2022__1667232327  18:05:27 heure sur afficheur

//1-sans canbus: renommer PIN_CS_CAN constantes.h (a essayer)
//2-sans wifinfo renommer WITHWIFINFO wifinfo.h (a essayer)

//  *****************************************************************
//  *Pour programmer par l'usb:                                     *
//  *  pb si toutes les entrées débranchées sauf liaison can bus    *
//  *a vérifier si debrancher que can bus                           *
//  *****************************************************************

//version carte nodemcu 3.0.2 probleme spiffs
//install version 2.7.4 probleme spiffs
//install version 2.6.0 probleme spiffs
//install version 2.5.2 spiffs ok multiple definition of `time' renommé time to time_1 to SNTPTime.cpp
//install version 2.5.0 spiffs ok multiple definition of `time'
//  *********************************************************************************************
//  *            programmation:ne pas oublier cpu frequency 160Mhz et flash size 4M(1M OTA)     * 
//  *    sinon OTA ne fonctionne plus ni le site smt160 si 80Mhz                                *
//  *paramétrage arduino                                                                        * 
//  *type de carte NodeMcu 0.9                                                                  *
//  *cpu frequency 160Mhz                                                                       *
//  *flash size 4M(1M OTA)                                                                      *
//  *rename traitement.ino.bin to traitement_tempo_vmc.ino.bin for upload with "site teleinfo"  *
//  *C:\Users\mireille\AppData\Local\Temp\arduino_build_xxxxxxx\traitement.ino.bin              *
//  *********************************************************************************************
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
//#####################################################################  
// 06/2022  SPIFFS to LITTLEFS
//* installation esp8266:https://arduino-esp8266.readthedocs.io/en/3.0.2/installing.html?highlight=python
//* problème python3:
//*C:\Users\mireille\Documents\Arduino>mklink python3.exe C:\Users\mireille\AppData
//*\Local\Arduino15\packages\esp8266\tools\python\3.7.2-post1\python.exe
//*Lien symbolique créé pour python3.exe <<===>> C:\Users\mireille\AppData\Local\Ar
//*duino15\packages\esp8266\tools\python\3.7.2-post1\python.exe
//**************************************************************************************************
//version esp8266 3.0.2
//version arduino 1.8.19
//
//passage du systeme de fichier de SPIFFS a littleFS
//transfert du dossier data via plugin arduino si SPIFFS:
//  C:\Program Files (x86)\Arduino\tools\ESP8266FS\tool\esp8266fs.jar
//transfert du dossier data via plugin arduino  si littleFS:
//  C:\Program Files (x86)\Arduino\tools\ESP8266LittleFS\tool\esp8266littlefs.jar
//  arduino->menu outils->ESP8266 littleFS data upload
//probleme de password si port=adresse IP avec le plugin esp8266fs.jar d'origine, telecharger:
//https://github.com/877dev/arduino-esp8266littlefs-plugin
//**************************************************************************************************
//sur une autre appli(mesure Temperature,la page web fonctionnait mais aucune data.
//au niveau de l'outils de développement du navigateur, dans la liste des fichiers n'aparaissait pas fonctions.js.
//je l'ai transféré dans le dossier js, ajouté js dans index.htm (derniere ligne)
//<script type="text/javascript" src="js/fonctions.js"></script> et c'est ok pourquoi ??
//lecture de la liste des fichiers par http://192.168.1.XX/spiffs.json

//********************************************************************
//version wifinfo syslog d'origine:352 392 bytes
//********************************************************************
//9/12/2021
//remplacer les XXXX par ptec
//emettre le dernier compteur du jour qu'on quitte
//ajouter I2S.h dans C:\Program Files (x86)\Arduino\libraries\NeoPixelBus-master\src\internal
//***************************10/10/2022*****************************************
//modification du traitement des commandes temporisees
//decompte dans traitement pose trop de probleme avec la boucle de transfert entre affichage
//et traitement => transfert du decompte dans affichage.

//dans traitement:transfert de la duree programmee vers affichage(decompteTempoArretMarcheForce).
//dans affichage:
//    memorisation de la commande en cours
//    emission de la commande vers traitement.----->traitement->VMC.setLeMode
//      reception d'une commande temporisee<--------traitement->VMC.getLeMode
//        demarrage du decompte
//          attente de la fin de decompte tout en attendant une eventuelle nouvelle commande.
//          -soit nouvelle commande avant fin tempo
//          -soit activation de la commande memorisee
//    la duree de temporisation est calculee sur le temps recu HMS(voir le passage a minuit 
//    combine avec la duree min vmc).

//ensuite il faut voir buf[MESSAGE_TYPE_5::FORCAGE_MODE] = 0; // VMC.getForcageMode();
//pour traitementVMCHiver  mode= soit BIDON soit AUTO
//###################################watchdog##################################
//https://sigmdel.ca/michel/program/esp8266/arduino/watchdogs3_fr.html  
//https://sigmdel.ca/michel/program/esp8266/arduino/watchdogs_fr.html
/*
-1 watchdog materiel
	-Actif par defaut
	-periode 6sec.
	-reactivation:pas besoin d'instruction incluse dans loop(),yield() et delay()
-2 watchdog logiciel
	-Actif par defaut
	-desactivation:ESP.wdtDisable();
	-periode <6sec : 3.2 ou 1.5 en fonction de l'init ?
	-reactivation:pas necessare:instruction incluse dans loop(),yield() et delay()
-3 watchdog user
    -Ce site recommande de ne pas toucher a ces 2 watchdog mais plutot d'ajouter
	un watchdog "user"

*/
// 

//###################################includes##################################  
//#include "constantes.h"   ici,COMP_CAN_BUS pas vu??? il faut apres Wifinfo.h???
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "Wifinfo.h"
#include "constantes.h"
#include "mySNTP.h"
#include "mySyslog.h"
#include "myOTA.h"
#include "capteur.h"
#include "smt160.h"
#include "dht.h"
#include "vmc.h"
#ifdef TA12
#include "tA12.h"
#endif
#include "simuTempo.h"
#include "LibTeleinfo.h"
#include "enregistrement.h"
#ifdef ALARME
#include "alarme.h"
#endif
#include "interRelais.h"
#include "ledsRGBSerial.h"
#include "webServer.h"
#include "webclient.h"
#include "config.h"
#include "myWifi.h"
#include "buzzer.h"
#include "myTinfo.h"
//###################################defines################################## 
#ifdef PIN_CS_CAN
	#include "canBus.h"
	can_bus CAN_BUS;
#endif

myTinfo MYTINFO;

enregistrement ENREGISTREMENT;
#ifdef ALARME
	myAlarme MYALARMEGARAGE;
	myAlarme MYALARMEPORTAIL;
#endif
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
#ifdef TA12
	ta12 TA12;
#endif
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
webClient WEBCLIENT;
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

//  system_update_cpu_freq(160);

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
#ifdef TA12
	CAPTEURIINST.initCapteur(0, 1, ID_CLASSES::ID_TA12);
#endif
  DHTCUISINE_T.initCapteur(CONFIGURATION.config.tempo.cor_temp_cuis_dixieme_degres, 10, ID_CLASSES::ID_DHTCUISINE_T);
  DHTCUISINE_H.initCapteur(CONFIGURATION.config.tempo.cor_hum_cuis_pourcent, 1, ID_CLASSES::ID_DHTCUISINE_H);
  DHTSDB.initCapteur(PIN_CAPTEUR_TEMP_HUMIDITE_SDB, ID_CLASSES::ID_DHTSDB);
#ifdef TA12
  TA12.initCapteur(PIN_CAPTEUR_ANA_COURANT, 1);
  TA12.setSeuilPetiteVitesse(20);
  TA12.setSeuilGrandeVitesse(39);
#endif
  DHTSDB.DHT_H.initCapteur( CONFIGURATION.config.tempo.cor_hum_sdb_pourcent, 1, ID_CLASSES::ID_DHTSDB_H);
  DHTSDB.DHT_T.initCapteur(CONFIGURATION.config.tempo.cor_temp_sdb_dixieme_degres, 10, ID_CLASSES::ID_DHTSDB_T);
  WEBSERVER.initSpiffs();  
  WEBSERVER.initServeur();
#ifdef  TELEINFO_RXD2 
  DebuglnF("Fin des traces consoles voir avec syslog,la teleinfo est recue sur RXD2");
  DebuglnF("Changement de vitesse des traces consoles voir avec syslog,la teleinfo est maintenant recu sur RXD0 a la place de l'USB(OU RXD2 si Serial.swap())");
#else
	#ifdef MODE_HISTORIQUE
		DebuglnF("Les traces consoles continue de fonctionner sur TXD0 à 1200 bauds ou VITESSE_SIMUTRAMETEMPO si SIMUTRAMETEMPO");
	#else
		DebuglnF("Les traces consoles continue de fonctionner sur TXD0 à 9600 bauds ou VITESSE_SIMUTRAMETEMPO si SIMUTRAMETEMPO");
	#endif
	DebuglnF("La teleinfo est recue sur RXD0");
#endif
#ifdef MODE_HISTORIQUE
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
#ifdef 	WITHWIFINFO
	MYTINFO.init();    //MODE_HISTORIQUE
#endif
  uint8_t timeout = 5;
  while (WIFINOOKOU && timeout)
  {
	  delay(500);
	  DebugF(".");
	  --timeout;
  }
MYSNTP.init();
ENREGISTREMENT.init();
#ifdef ALARME
	MYALARMEGARAGE.init(INDICEALARMES::GARAGE);
	MYALARMEPORTAIL.init(INDICEALARMES::PORTAIL);
#endif
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
bool passeMinuit = false;
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
//#ifdef COMP_CAN_BUS                         transférer dans la partie sequencee a 1 seconde essai de reprise erreur can bus en reception
//    if(CAN_BUS.traiteReception())
//      WEBSERVER.incNb_reinit();
//#endif
//************************************************1 fois par seconde*************************************************************************************
  // Only once task per loop, let system do its own task
 if (MYSNTP.getCycle1Seconde()) {

#ifdef COMP_CAN_BUS
    if(CAN_BUS.traiteReception())
      WEBSERVER.incNb_reinit();
#endif
  
	WEBSERVER.handleClient();
	MYOTA.handle();
	//**************************************Traitement enregistrement****************************************************

	//enregistrement des 6 compteurs à minuit pour graphiques journaliers

	bool minuit=MYSNTP.TestSiMinuit();
#ifdef 	WITHWIFINFO
	boolean CgtCompteur = MYTINFO.getEtResetCgtCompteur(); 
#endif
	if(minuit )    //&& CgtCompteur
	{
		if(!passeMinuit)
			ENREGISTREMENT.clrPremierEnregistrement();	//envoi des 6 compteurs
		passeMinuit = true;
		RELAIS.razNbSecondeActiveJourCourant();
		if(ENREGISTREMENT.getpremiersEnregistrement()>=6)  //les 6 ont étés envoyés
		{
			MYSNTP.clrMinuit();
			passeMinuit = false;
		}
	}

	/*ENREGISTREMENT.testAcquitementMessage();*/
#ifdef 	WITHWIFINFO
	ENREGISTREMENT.TRAITEENREGISTREMENT(MYTINFO.getNouvelleTrame(), CgtCompteur,dureeMax);
#else
	ENREGISTREMENT.TRAITEENREGISTREMENT(false, false,dureeMax);
#endif
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
#ifdef ALARME
	MYALARMEGARAGE.testReceptionAlarme();
	MYALARMEPORTAIL.testReceptionAlarme();
#endif
//**************************************Traitement des sorties****************************************************
#ifdef COMP_CAN_BUS
	if(CAN_BUS.TRAITEMENTEMISSIONCAN(CgtCompteur))
	    WEBSERVER.incNb_reinit();
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
 	if (TINFO.getReinit())   //pas utilise
	{
    //Some polluted entries have been detected in Teleinfo ListValues
		//need_reinit=false;
		//WEBSERVER.incNb_reinit();    //account of reinit operations, for system infos commente pour test initCan
		MYTINFO.init();   //MODE_HISTORIQUE//Clear ListValues, buffer, and wait for next STX
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
