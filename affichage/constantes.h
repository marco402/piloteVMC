// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Constantes pour affichage.
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (https://marco40github.wixsite.com/website))
//
// History : V1.00 2018-03-23 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef CONSTANTES_H
#define CONSTANTES_H
#include <Arduino.h>

#define NO_TRAITMODE  //before constantesPartagees.h
#define ALARME     //probleme de memoire avec ALARME  22496 OK 22616 PB T et H DHT
#define NO_HORLOGETM1650  //Attention cablage different avec TM1637
#define HORLOGETM1637
#include "constantesPartagees.h" 
//################################################define##########################################################
//****************************************pins entrées sorties****************************************************
//RX													      0	
//TX													      1	
//INT0													    2
// marc a voir si remplacé par PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE #define PIN_DHT22										3		//inversion smt160 et mq7 voir schéma pcb
#define TFT_RS											4	    //
#define XXPIN_LED_JOUR_NUIT						5		//passer sur 5 manque R14 utilise R13

//There are six pins from the set of digital pins that are PWM (Pulse Width Modulation) pins, numbered as 3, 5, 6, 9, 10, and 11.
#define TFT_LEDA							      5	    //PWM via 2n2222 and 2N2907   pins PWM: 3, 5, 6, 9, 10, and 11.
#define PIN_CS_CAN								  7		//sur test receive blink 15µs haut 15µs bas
#define TFT_CS											8	    //
#define PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE				9
//#define               								10
#define TFT_MOSI										11		//SPI	MOSI	sur test receive blink 0.5µs bas  1µs haut 1µs bas
//														12		//SPI	MISO	sur test receive blink
#define TFT_SCLK										13		//SPI	CLK		PIN_LED_CARTE_PRO_MINI
#define TFT_RESET										-1      //3.3v via 1kohm
//libre													A0		//14
#define PIN_POUSSOIR_MODE						A1		//15
#define PIN_LED_RGB									A2		//16
#define PIN_BUZZER_NUM							A3		//17
//#ifdef HORLOGETM1650 //|| HORLOGETM1637
	#define CLK											  A5		//18 SDA I2C 
	#define DIO											  A4		//19 SCL I2C
//#endif
//#define TFT_DC										A4		//18 SDA I2C 
//#define TFT_CS										A5		//19 SCL I2C
//libre													A6		//20
//libre													A7		//21
////################################################enum##########################################################
enum COULEUR_JOUR { COULEUR_JOUR_BLEU = 0, COULEUR_JOUR_BLANC , COULEUR_JOUR_ROUGE ,COULEUR_JOUR_INIT };
enum LES_LEDS_RGB { LES_LEDS_RGB_LED_DEMAIN,LES_LEDS_RGB_LED_JOUR,LES_LEDS_RGB_LED_JOUR_NUIT,FIN_LED}; 
enum ARRET_MARCHE { ARRET_REL = 0, MARCHE_REL };
#ifdef ALARME
   enum CODES_ALARME { DOOR_OPEN_WITH_ALARME = 1, DOOR_CLOSE, DOOR_OPEN_WITHOUT_ALARME, HEARTBEAT, HEAD_MESSAGE, AQUITEMENT };
#endif
#define NB_LEDS_RGB LES_LEDS_RGB::FIN_LED
//################################################static const##########################################################
//################################################structures##########################################################
struct etatLeds {
	unsigned char
		aujourdhui : 2,   //lsb
		demain : 2,
		chauffage : 1,
		cumulus : 1,
		present : 1,
		jourNuit : 1;
};
struct struct_reception {
	//etatLeds etatDesLeds;	
	unsigned char etatLeds;
	unsigned char heures;
	unsigned char minutes;
	unsigned char secondes;
	unsigned char mode;
	unsigned char etat;
	uint8_t Rbuzzer=0;
	//int8_t puissanceVMC=0;
	int16_t temperature_cuis_aff;
	int16_t humidite_cuis_aff;
	int16_t temperature_sdb_aff;
	int16_t humidite_sdb_aff;
	int16_t temperature_ext_aff;
	boolean infos;
	boolean distants;
	boolean locaux;
	boolean infos2;
	uint8_t NbMessage=0;
	uint16_t NbMinuteActiveJourCourant;
	int8_t decompteDelaiCgtVitesse;
	unsigned long decompteTempoArretMarcheForce;
	int8_t etatWifi;
	uint8_t luminositeeLeds = 10;
	uint8_t arret_marche;
	uint8_t seuilAuto;
	uint8_t casAuto;
#ifdef ALARME
	uint8_t alarmeGarage = 0;
	uint8_t alarmePortail = 0;
#endif
	uint16_t dureeForcageSec = 0;
	MODES forcageMode;
};
//################################################union##########################################################
union CANconv {
	int16_t capteur;
	int8_t b[3]  ;
};
union leds {
	struct etatLeds etatCourant;
	unsigned char etat=0;
};
//################################################fin##########################################################
#endif
