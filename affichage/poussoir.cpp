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

poussoir::poussoir()
{
	pinMode(PIN_POUSSOIR_MODE, INPUT);
}
//appel� a chaque loop de affichage.ino
unsigned long  poussoir::getTempsMilliCommandeTemporisees(void)
{
	return tempsMilliCommandeTemporisees;
}
boolean poussoir::traitement(int16_t dureeForcageSec)
{
//            Serial.print(F("dureeForcageSec: "));
//           Serial.println(dureeForcageSec);
//#ifdef TRAITMODE
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
 //           Serial.print(F("leMode: "));  Serial.println(leMode);
 //           Serial.print(F("forcageMode: "));  Serial.println(forcageMode);
 //           
	//if (leMode == MODES::HIVER && forcageMode != MODES::DERNIER)  //le logiciel distant traitement indique si un forcage est en cours pour mode hiver
	//{
 //   leMode = forcageMode;  
 //   memoModes = leMode;  
	//}
//	//on garde la possibilitee de changer de mode pendant le forcage
//#endif
//            Serial.print(F("tempsMilliCommandeTemporisees: "));
//           Serial.println(tempsMilliCommandeTemporisees);
//            Serial.print(F("milli: "));
//           Serial.println(millis());

  if (tempsMilliCommandeTemporisees > 0 && tempsMilliCommandeTemporisees < millis())
	{
    Serial.print(F("memoModes: ")); Serial.println(memoModes);
		leMode = memoModes ;
		tempsMilliCommandeTemporisees = 0;
	}
//	static boolean  transitoirePoussoir=false;
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
			if (tempInt>(MODES::DERNIER-1))
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
        memoModes = leMode;      
				leMode=modeTransitoire;	//validation du nouveau mode, lemode devient diff�rent de memoModes-->InitialisationMode
//#ifdef TRAITMODE

				tempsMilliCommandeTemporisees = 0;   //arret d'une commande temporisee si en cours
//#endif        
				//Serial.print("modeTransitoire"); Serial.println(modeTransitoire);
				transitoirePoussoir=false;
				AFFICHEUR.setchangeMode(false);
//#ifdef TRAITMODE
				//traitement des modes temporises
				switch (leMode)
				{
				case MODES::TEMPO_ARRET:
				case MODES::TEMPO_MARCHE_PV:
				case MODES::TEMPO_MARCHE_GV:
				    //memoModes = leMode;  
				  //leMode = forcageMode;
					//lancement du decompte
					tempsMilliCommandeTemporisees =(unsigned long) millis() + (unsigned long)dureeForcageSec * 1000;
				  break;
				}
//#endif
			}
		}
	}
	return buzzer;
}
MODES poussoir::getLeMode()
{
return leMode;
}
boolean poussoir::getTransitoirePoussoir(void)
{
  return transitoirePoussoir;
}
MODES poussoir::getLemodeTransitoire(void)
{
  return modeTransitoire;
}

void poussoir::setLemode(unsigned char leMode)
{
	tempsMilli = 0;               //forcage de la prise en compte pour le retour en mode auto apres forcage
	//Serial.print("setLemode"); Serial.println(leMode);
	this->leMode = (MODES)leMode;
}
void poussoir::clearLeMode(void)
{
	this->leMode = MODES::DERNIER;
}
//#ifdef TRAITMODE
//boolean startmodeTempo=false;
//void poussoir::testModeForce(int16_t decompteTempoArretMarcheForce)      // , uint16_t duree_forcage_sec)
//{
//	//if (decompteTempoArretMarcheForce > 0)
//	//	decompteTempoArretMarcheForce -= 1;
//
//	//else if(leMode != memoModes)
//	//{
//	switch (leMode)
//		{
//		case MODES::TEMPO_ARRET:
//		case MODES::TEMPO_MARCHE_PV:
//		case MODES::TEMPO_MARCHE_GV:
//    if(decompteTempoArretMarcheForce>0)
//    {
//      startmodeTempo=true;
//    }
//		else if (decompteTempoArretMarcheForce == 0 && startmodeTempo == true)
//			{
//      Serial.print(F("leMode avant "));Serial.println(leMode);
//		  leMode = memoModes;
//      startmodeTempo=true;
//      Serial.print(F("leMode apres "));Serial.println(leMode);
//			}
//			//decompteTempoArretMarcheForce = duree_forcage_sec;
//			break;
//		}
//	//}
//}
//#endif
