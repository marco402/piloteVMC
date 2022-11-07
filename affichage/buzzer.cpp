// **********************************************************************************
// Programme affichage arduino pro mini Pilotage VMC et TEMPO->gestion du buzzer
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
//Using library Newtone version 0.0.0
//
// **********************************************************************************
#include <Arduino.h>
#include <newtone.h>
#include "canBus.h"
#include "dht.h"
#include "constantes.h"
#include "buzzer.h"
 buzzer::buzzer()
 {
	pinMode(PIN_BUZZER_NUM,OUTPUT);
	leBuzzer=false;
 }
void buzzer::traitement(struct_reception reception)
{
	if( reception.Rbuzzer== ERREURS::E_TA12)
		NewTone(PIN_BUZZER_NUM, 2400, 500);
	leBuzzer=false;
}
void buzzer::test(void)const
{
	NewTone(PIN_BUZZER_NUM, 2400, 500);
}
void buzzer::setBuzzer(boolean etat)
{
if (etat)    //pour ne pas annuler les pr�cedents du cycle
	leBuzzer=true;   //true->beep
}
uint8_t buzzer::getBuzzer() const
{
	return leBuzzer;
}
void buzzer::clrBuzzer()
{
	leBuzzer = false;
}
