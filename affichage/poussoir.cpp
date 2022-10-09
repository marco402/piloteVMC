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
//appel� a chaque loop de affichage.ino
boolean poussoir::traitement(MODES forcageMode)
{
#ifdef TRAITMODE
	//struct_reception structReception;
//switch (leMode)
//    {
//    case MODES::TEMPO_ARRET:
//    case MODES::TEMPO_MARCHE_PV:
//    case MODES::TEMPO_MARCHE_GV:
//      memoModes = leMode;  
//      leMode = forcageMode;
//      break;
//    }
	if (forcageMode != MODES::BIDON)  //le logiciel distant traitement indique si un forcage est en cours
	{
    leMode = forcageMode;  
    memoModes = leMode;  
	}
	//on garde la possibilitee de changer de mode pendant le forcage
#endif
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
			if (tempInt>(MODES::BIDON-1))
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
				leMode=modeTransitoire;	//validation du nouveau mode, lemode devient diff�rent de memoModes-->InitialisationMode
#ifdef TRAITMODE
				memoModes = leMode;
#endif
				//Serial.print("modeTransitoire"); Serial.println(modeTransitoire);
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
	tempsMilli = 0;               //forcage de la prise en compte pour le retour en mode auto apres forcage
	//Serial.print("setLemode"); Serial.println(leMode);
	this->leMode = (MODES)leMode;
}
void poussoir::clearLeMode(void)
{
	this->leMode = MODES::BIDON;
}
#ifdef TRAITMODE
boolean startmodeTempo=false;
void poussoir::testModeForce(int16_t decompteTempoArretMarcheForce)      // , uint16_t duree_forcage_sec)
{
	//if (decompteTempoArretMarcheForce > 0)
	//	decompteTempoArretMarcheForce -= 1;

	//else if(leMode != memoModes)
	//{
	switch (leMode)
		{
		case MODES::TEMPO_ARRET:
		case MODES::TEMPO_MARCHE_PV:
		case MODES::TEMPO_MARCHE_GV:
    if(decompteTempoArretMarcheForce>0)
    {
      startmodeTempo=true;
    }
		else if (decompteTempoArretMarcheForce == 0 && startmodeTempo == true)
			{
      Serial.print("leMode avant ");Serial.println(leMode);
		  leMode = memoModes;
      startmodeTempo=true;
      Serial.print("leMode apres ");Serial.println(leMode);
			}
			//decompteTempoArretMarcheForce = duree_forcage_sec;
			break;
		}
	//}
}
#endif
