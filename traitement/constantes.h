// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO Traitement du message pour  enregistrement
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
// History : V1.00 2018-03-23 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef CONSTANTE_H
#define CONSTANTE_H
#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "Wifinfo.h"

#include "constantesPartagees.h"   //fichier identique avec affichage,il faut le charger dans les 2 projets pour qu'il soit transféré dans le dossier de generation(user...)



//################################################define##########################################################
#define WIFIOK ((WiFi.status() == WL_CONNECTED) && (wifi_station_get_connect_status() == STATION_GOT_IP))
#define WIFINOOKET ((WiFi.status() != WL_CONNECTED) && (wifi_station_get_connect_status() != STATION_GOT_IP))
#define WIFINOOKOU ((WiFi.status() != WL_CONNECTED) || (wifi_station_get_connect_status() != STATION_GOT_IP))
#define FPM_SLEEP_MAX_TIME 0xFFFFFFF
//***********************partie extraite de  wifinfo Written by Charles-Henri Hallard (http://hallard.me)**********************************
// value for HSL color
// see http://www.workwithcolor.com/blue-color-hue-range-01.htm
#define COLOR_RED             0
#define COLOR_ORANGE         30
#define COLOR_ORANGE_YELLOW  45
#define COLOR_YELLOW         60
#define COLOR_YELLOW_GREEN   90
#define COLOR_GREEN         120
#define COLOR_GREEN_CYAN    165
#define COLOR_CYAN          180
#define COLOR_CYAN_BLUE     210
#define COLOR_BLUE          240
#define COLOR_BLUE_MAGENTA  275
#define COLOR_MAGENTA	    300
#define COLOR_PINK		    350
#define rgb_brightness       1        //50				//50 trop   25 trop 10 trop 0 eteint
#define BLINK_LED_MS   50 // 50 ms blink

//**************************partie liée a la partie tempo  Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)**************

//											pins nodemcu 1.0	pins esp8266

//										D3			//GPIO0				GPIO0	(nodemcu:12k au +3.3V)flash(voir spiCs2) high run mode low flash mode
//										D10			//GPIO1				TXD		emission vers usb(voir spiCs1)

//PIN_LED_RGB

//										D9			//GPIO3				RXD		reception de usb puis reception teleinfo
#define PIN_RELAIS_VITESSE				D1			//GPIO5				CD carte mémoire SD		GPIO5	leds inline rgb	 pin récupérable si pas de leds locales	
#define PIN_CAPTEUR_TEMP_HUMIDITE_SDB	D2			//GPIO4				GPIO4	int spi   puis dht22 sdb //PIN_INT_SPI
//#define Libre pb GPIO0				D3			//					GPIO0   int spi ATTENTION pas cablée
#define PIN_CS_CAN						D4			//GPIO2				GPIO2	(nodemcu:12k au +3.3V)txd1	simu teleinfo      led bleue    schema sur D3...
			
//GPIO9				GPIO9	sd2	
//GPIO10			GPIO10	sd3      

//										D6			//GPIO12			GPIO12	HMISO(D6)	chauffage	
//										D7			//GPIO13			GPIO13	HMOSI(D7)	rxd2	0 vers   3.5 volts a vide	

//#define PIN_CAPTEUR_TEMP_HUMIDITE_SDB	D5			//GPIO14			GPIO14	HCLOCK(D5)	cumulus 0 vers	 1.8(lolin) ou 2(0.9) volts a vide						 a voir si led rouge
#define PIN_RELAIS_MARCHE_ARRET			D8			//GPIO15		    GPIO15	(nodemcu:12k au 0V)HCS(D8)		txd2	attention au démarrage pb sur adaptateur de niveau csCan				3 volts vers 0 à vide		


#define PIN_CAPTEUR_TEMP_EXT			D0			//GPIO16			GPIO16	user	wake(ok si pas de sleep mode)??		avec clock à 100000 et send blink  :7.5µs/15µs							a voir GPIO16 avec la led bleu
//MOSI										//GPIO8				MOSI	sd1						
//MISO										//GPIO7				MISO	sd0						
//CLK										//GPIO6				SCLK	clk
//																RST
#define	PIN_CAPTEUR_ANA_COURANT			A0								
#define LONGMAXMOT 12
//################################################enum##########################################################
enum VITESSE_RELAIS { LENT_REL = 0, RAPIDE_REL };
enum ARRET_MARCHE { ARRET_REL = 0, MARCHE_REL };
enum ADRESS_STATIC{ TEMPOVMC = 101, REGULTEMP, };
enum TEMPO_UTILISE { ETU_HCJB, ETU_HCJW, ETU_HCJR, ETU_HPJB, ETU_HPJW, ETU_HPJR, ETU_PTEC, ETU_DEMAIN, ETU_IINST };
enum ETAT_JOUR { ETAT_JOUR_BLEU = 0, ETAT_JOUR_BLANC, ETAT_JOUR_ROUGE, ETAT_JOUR_INIT, ETAT_JOUR_PRESENT = 0, ETAT_JOUR_ABSENT = 16, ETAT_JOUR_JOUR = 0, ETAT_JOUR_NUIT = 3, ETAT_JOUR_INCONNU = 10 };
enum COULEURS { C_JOUR_BLEU = COLOR_BLUE, C_JOUR_BLANC = COLOR_ORANGE_YELLOW, C_JOUR_ROUGE = COLOR_RED, C_CHAUFFAGE = COLOR_RED, C_CUMULUS = COLOR_RED, C_JOUR = COLOR_CYAN, C_NUIT = COLOR_GREEN, C_PRESENT_ABSENT = COLOR_RED, C_WIFI = COLOR_GREEN, C_OTA = COLOR_ORANGE, C_OTA_ERROR = COLOR_PINK, C_SPIFF = COLOR_MAGENTA };
enum  LES_LEDS_EN_SERIE { LED_BWRJOUR, LED_BWRDEMAIN, LED_JOURNUIT, NBLEDENSERIE };  //, LED_PRESENTABSENT LED_CHAUFFAGE, LED_CUMULUS, 
enum ID_CLASSES { ID_TEMPEXT, ID_DHTCUISINE_T, ID_DHTCUISINE_H, ID_DHTSDB_T, ID_DHTSDB_H , ID_TA12, ID_DHTSDB};
enum ETAT_RELAIS { INCONNU_ER = 0, ETEINT = 1, ALLUME = 0, CUM_ETEINT = 0, CUM_ALLUME = 32, CH_ETEINT = 0, CH_ALLUME = 64 };

enum  VAL_TYPE_JOUR { BLEU_J = 0, BLANC_J = 4, ROUGE_J = 8, BLEU_DEM = 0, BLANC_DEM = 1, ROUGE_DEM = 2, INCONNU_J = 10 };
enum ERREURS {E_TEMPEXT=1, E_DHTCUISINE=2, E_DHTSDB=4, E_TA12=8, E_CAN_BUS_TRAIT=0x10, E_CAN_BUS_AFF=0x20, E_TINFO=0x30};

static const uint8_t   TEMPO_CGT_ETAT_RELAIS = 5;
static const  uint16_t  REINIT = 65000;                    //ne pas mettre au max, pour ne pas revenir a 0 sur le +1

//problem PROGMEM conflict __c
static const char TableauTempoName[][8] = { "BBRHCJB","BBRHCJW","BBRHCJR","BBRHPJB","BBRHPJW","BBRHPJR","PTEC","DEMAIN","IINST" };
static const char tableauPtec[][5] = { "HCJB", "HCJW", "HCJR", "HPJB", "HPJW", "HPJR" };

PROGMEM const char MODES_AFF[][11] = { "Arret","Lent","Rapide","Auto","Force pv","Force gv","Forc arret","AT CAN BUS","cas inex" }; 
PROGMEM const char ETAT_REL_AFF[][7] = { "Arret","Marche" };
PROGMEM const char VIT_REL_AFF[][7] = { "Lent","Rapide" };
//################################################structures##########################################################

//################################################union##########################################################
union CANconv {
	int16_t capteur;
	int8_t b[2];
};
#endif
