// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Gestion du buzzer
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
#include "buzzer.h"
ICACHE_FLASH_ATTR buzzer::buzzer(void)
 {
	leBuzzer=false;
 }
void buzzer::setBuzzer(boolean etat)
{
if (etat)    //pour ne pas annuler les precedents du cycle
	leBuzzer=true;
}
uint8_t buzzer::getBuzzer(void) const
{
	return leBuzzer;
}
void buzzer::clrBuzzer(void)
{
	leBuzzer = false;
}
