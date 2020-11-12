// **********************************************************************************
// Programme affichage  arduino pro mini  Pilotage VMC et TEMPO->Pilotage d'une led "classique"
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
#include <Arduino.h>
#include "constantes.h"
#include "canBus.h"
#include "ledsClassiques.h"
ledsClassiques::ledsClassiques()
{
	pinMode(PIN_LED_JOUR_NUIT, OUTPUT);
}
enum ETAT_JOUR {  ETAT_JOUR_JOUR = 0, ETAT_JOUR_NUIT = 1, ETAT_JOUR_INCONNU = 10 };
boolean ledsClassiques::traitement(struct_reception reception)
{
	static unsigned char memoJourNuit = 0;       //0:jour,1:nuit 
	static uint8_t cptNbBeep = 0;
	boolean retour = false;
	if(reception.infos)
	{
 		union leds etatDesLeds;
		etatDesLeds.etat=reception.etatLeds;
		digitalWrite(PIN_LED_JOUR_NUIT,etatDesLeds.etatCourant.jourNuit);
		if (cptNbBeep > 0)
		{
			cptNbBeep -= 1;
			retour= true;
		}
		else if ((memoJourNuit == 0) && (etatDesLeds.etatCourant.jourNuit == 1))  //passage en heure creuse
		{
			cptNbBeep = 4;
			retour= true;
		}
		memoJourNuit = etatDesLeds.etatCourant.jourNuit;	
	}
	return retour;
}
