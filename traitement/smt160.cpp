// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->mesure de température extérieur avec sonde smt160
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
#include "Wifinfo.h"
#include "mySyslog.h"
#include "constantes.h"
#include "smt160.h"
#define NO_DEBUGSMT160
ICACHE_FLASH_ATTR smt160::smt160(void)
{}
ICACHE_FLASH_ATTR smt160::smt160(uint8_t pin,int16_t  correction,uint8_t ajustDecimal)
{	
	this->pin=pin;
	this->correction=correction;
	this->ajustDecimal=ajustDecimal;
	pinMode(pin, INPUT);
}
ICACHE_FLASH_ATTR smt160::smt160( int16_t  correction, uint8_t ajustDecimal)
{
	this->correction = correction;
	this->ajustDecimal = ajustDecimal;
	pinMode(pin, INPUT);
}
//****************************VERSION SANS INTERRUTION****************************************
//nécessaire si iniatialisation de la config
void ICACHE_FLASH_ATTR smt160::initCapteur(uint8_t pin, int16_t  correction, uint8_t ajustDecimal, uint8_t idClasse)
{
	this->pin = pin;
	this->correction = correction;
	this->ajustDecimal = ajustDecimal;
	pinMode(pin, INPUT);
	this->idClasse = idClasse;
}
void smt160::setCorrection(int16_t correction)
{
	correction = correction;
}
boolean smt160::lectureCapteur()   //
{
#ifdef DEBUGSMT160
	static int16_t premierPassage = 0;  //pour debug
#endif
	unsigned long  haut = 0; 
	unsigned long  bas = 0;
	float cumul=0;
	int8_t nb=0;
	for (int8_t i = 0; i < 10; i++)
	{
		noInterrupts();
		haut = pulseIn(pin  , HIGH, 1000L);  //300 a la place de 1000
		interrupts();
#ifdef DEBUGSMT160
		if ( premierPassage == 0)
		{
		DebugF("Haut smt160= ");Debugln((long)haut);   //remplacé : par = pour voir si plus dans colonne tag mais dans colonne message
		}
#endif
		if (haut==0)
		{
			compteurBuzzer+=1;
			if (compteurBuzzer >= TEMPO_BIP)
			{
				compteurBuzzer = 0;
				erreur = ERREURS::E_TEMPEXT;
			}
			return false;  //on reste sur la dernière valeur pour le cycle en cours
		}
		noInterrupts();
		bas = pulseIn(pin, LOW, 1000L);  //300 a la place de 1000
		interrupts();
#ifdef DEBUGSMT160
		if (premierPassage == 0)
		{
			DebugF("Bas smt160: ");	Debugln((long)bas);
		}
#endif
		if (bas == 0)
		{
			erreur = ERREURS::E_TEMPEXT;
			return false;			//on reste sur la dernière valeur pour le cycle en cours
		}
		//entrée du régulateur----> 7.82V  ,sortie du régulateur:4.94---->smt160---->4.58 manque 1°
		float  rapportCycle =(float) ((float)haut/(float)(haut + bas));   
		float T=(rapportCycle - 0.320) / 0.00470;
		if (T > -40 && T < 80)
		{
			cumul+=T;
			nb+=1;
		}
	}
	cumul=(cumul/nb)*10.0; //  * 10 en dixième de degrés signés
#ifdef DEBUGSMT160
	if (premierPassage == 0)
	{
		DebugF("T: ");Debugln((long)cumul);
		//premierPassage += 1;
		if (premierPassage == 60)
			premierPassage = 0;
	}
#endif
	traiteMesure((int16_t)(cumul));
	compteurBuzzer = 0;
	return true;
	}
void smt160::simuMesure(int16_t capteur)
{
	traiteMesure(capteur);
}
