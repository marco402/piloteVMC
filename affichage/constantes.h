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
#include "constantesPartagees.h" 
//################################################define##########################################################
//****************************************pins entrées sorties****************************************************
//RX													0	
//TX													1	
//INT0													2
// marc a voir si remplacé par PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE #define PIN_DHT22										3		//inversion smt160 et mq7 voir schéma pcb
#define TFT_DC											4	//
#define PIN_LED_JOUR_NUIT								5		//passer sur 5 manque R14 utilise R13
#define PIN_LED_PRESENT_ABSENT							6	a voir libre //
#define PIN_CS_CAN										7		//sur test receive blink 15µs haut 15µs bas
#define TFT_CS											8	//
#define PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE				9
#define TFT_LEDA										10	//via 1kOhm pour extinction
#define TFT_MOSI										11		//SPI	MOSI	sur test receive blink 0.5µs bas  1µs haut 1µs bas
//														12		//SPI	MISO	sur test receive blink
#define TFT_SCLK										13		//SPI	CLK		PIN_LED_CARTE_PRO_MINI
#define TFT_RESET										-1
//libre													A0		//14
#define PIN_POUSSOIR_MODE								A1		//15
#define PIN_LED_RGB										A2		//16
#define PIN_BUZZER_NUM									A3		//17
//#define TFT_DC										A4		//18 SDA I2C 
//#define TFT_CS										A5		//19 SCL I2C
//libre													A6		//20
//libre													A7		//21
////################################################enum##########################################################
enum COULEUR_JOUR { COULEUR_JOUR_BLEU = 0, COULEUR_JOUR_BLANC , COULEUR_JOUR_ROUGE ,COULEUR_JOUR_INIT };
enum LES_LEDS_RGB { LES_LEDS_RGB_LED_DEMAIN,LES_LEDS_RGB_LED_JOUR ,FIN_LED};
enum ARRET_MARCHE { ARRET_REL = 0, MARCHE_REL };
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
	uint8_t Rbuzzer;
	int8_t puissanceVMC;
	int16_t temperature_cuis_aff;
	int16_t humidite_cuis_aff;
	int16_t temperature_sdb_aff;
	int16_t humidite_sdb_aff;
	int16_t temperature_ext_aff;
	boolean infos;
	boolean distants;
	boolean locaux;
	boolean infos2;
	uint8_t NbMessage = 0;
	uint16_t NbMinuteActiveJourCourant;
	int8_t decompteDelaiCgtVitesse;
	int16_t decompteTempoArretMarcheForce;
	int8_t etatWifi;
	uint8_t luminositeeLeds = 10;
	uint8_t arret_marche;
	uint8_t seuilAuto;
	uint8_t casAuto;
};
//struct etatLeds {
//	unsigned char aujourdhui : 2;   //lsb
//	unsigned char demain : 2;
//	unsigned char chauffage : 1;
//	unsigned char cumulus : 1;
//	unsigned char present : 1;
//	unsigned char jourNuit : 1;
//};
//################################################union##########################################################
union CANconv {
	int16_t capteur;
	int8_t b[3];
};
union leds {
	struct etatLeds etatCourant;
	unsigned char etat;
};
//################################################fin##########################################################
#endif