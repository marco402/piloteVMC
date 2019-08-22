// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Traitement des lesd RGB
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
//Using library NeoPixelBus-master version 2.3.4
//
// **********************************************************************************

#ifndef LEDSRGBSERIAL_H
#define LEDSRGBSERIAL_H

#include <Arduino.h>
#include "Wifinfo.h"
#include "constantes.h"

void LedOff(LES_LEDS_EN_SERIE led);  //a voir sorti de la classe pour rgb_ticker
//struct etatLeds {
//	unsigned char aujourdhui : 2;
//	unsigned char demain : 2;
//	unsigned char chauffage : 1;
//	unsigned char cumulus : 1;
//	unsigned char present : 1;
//	unsigned char jourNuit : 1;
//};
struct etatLeds {
	unsigned char
		aujourdhui : 2,
		demain : 2,
		chauffage : 1,
		cumulus : 1,
		present : 1,
		jourNuit : 1;
};
union leds {
	struct etatLeds etatCourant;
	unsigned char etat;
};
class lesLeds
{
public:
	void TRAITEMENTLEDS(void);
	void ledRGBON(LES_LEDS_EN_SERIE numLed, unsigned short hue);
	void ledRGBOFF(LES_LEDS_EN_SERIE numLed);
	void tempoLedOff(void);
	void initLed(void);
	unsigned char getEtatLeds(void);
	void testOn(void);
	void toutesOff(void);
private:
	void traitementLedsTempo(void);
	void traitementLedsTempoDemain(void);
	//void traitementLedsPresentAbsent(void);
	void traitementLedsRgb(ETAT_JOUR jour_nuit, ETAT_JOUR couleur_aujourdhui);
	Ticker rgb_ticker;

};
extern lesLeds LESLEDS;
#endif

