// **********************************************************************************
// Programme affichage  arduino pro mini  Pilotage VMC et TEMPO->Gestion d'un poussoir sensitif pour changer de mode VMC
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
#include "poussoir.h"
#include "st7735.h"
unsigned long tempsMilli = 0 ;
poussoir::poussoir()
{
	pinMode(PIN_POUSSOIR_MODE, INPUT);
}
//appelé a chaque loop de affichage.ino
boolean poussoir::traitement()
{
	static boolean  transitoirePoussoir=false;
	boolean buzzer=false;
	if ((millis()- tempsMilli) > 1000)
	{
		tempsMilli = millis();
		uint8_t LecturePoussoir=digitalRead(PIN_POUSSOIR_MODE);
		if (LecturePoussoir==HIGH)   //5v sur appui
		{
			//balayage des modes de fonctionnement.
			//passage par un mode transitoire pour prendre que le mode en cours au moment du relachement
			int tempInt=(int)modeTransitoire;
			tempInt++;
			if (tempInt>TEMPO_ARRET)
			{
				modeTransitoire= MODES::ARRET;
			}
			else
			{
				modeTransitoire=(MODES)tempInt;
			}
			if (!transitoirePoussoir)
				AFFICHEUR.setchangeMode(true);
			transitoirePoussoir=true;
			AFFICHEUR.setMode(modeTransitoire);
			buzzer=true;
		}
		else						//pas d'appui
			{
			if (transitoirePoussoir)	//relachement
			{
				leMode=modeTransitoire;	//validation du nouveau mode, lemode devient différent de memoModes-->InitialisationMode
				transitoirePoussoir=false;
				AFFICHEUR.setchangeMode(false);
			}
		}
	}
	return buzzer;
}
MODES poussoir::getLeMode()
{
return leMode;
}
void poussoir::setLemode(unsigned char leMode)
{
	leMode = (MODES)leMode;
}
void poussoir::clearLeMode(void)
{
	leMode = MODES::BIDON;
}