// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Traitement des lesd RGB
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
//
//Using library NeoPixelBus-master version 2.3.4
//
// **********************************************************************************

//� 160-->100n haut/front 100n/300n bas/front 100n pendant 65�s    reset 50�s  entre 2 leds
//� 80 -->200n haut/front 100n/600n bas/front 100n pendant 130�s   reset 50�s  entre 2 leds
//avec AAPA106,il faut 0.35�s+-150ns et 1.36�s+-150ns et reset 50�s

//leds ---------->WS2812
//#define CYCLES_800_T0H  (F_CPU / 2500000) // 0.4us    1Mhz?
//#define CYCLES_800_T1H  (F_CPU / 1250000) // 0.8us
//#define CYCLES_800      (F_CPU /  800000) // 1.25us per bit
//leds ---------->APA106
//#define CYCLES_800_T0H  (F_CPU / 1250000) // 0.35us    10Mhz?  pour APA106,il faut 0.35�s et 1.36�s
//#define CYCLES_800_T1H  (F_CPU / 420000) // 1.36us
//#define CYCLES_800      (F_CPU / 1250000)+(F_CPU / 420000) // 1.71us per bit
#include <Ticker.h>

#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "interRelais.h"
#include "LibTeleinfo.h"
#include "constantes.h"
#include "myTinfo.h"
#include "ledsRGBSerial.h"
//pas reussi � mettre dans la classe
#ifdef PIN_LED_RGB
  #include <NeoPixelBus.h>
	NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> rgb_led( NBLEDENSERIE , PIN_LED_RGB);  
#endif
	void lesLeds::TRAITEMENTLEDS(void)
	{
		if(MYTINFO.getNouvelleTrame())
		{ 
		traitementLedsTempo();  //aujourd'hui et jour/nuit
		traitementLedsTempoDemain();
		}
	}
	unsigned char lesLeds::getEtatLeds(void)
	{
		union leds etatDesLeds;
		etatDesLeds.etatCourant.aujourdhui = RELAIS.getAujourdhui() & 3;
		etatDesLeds.etatCourant.demain= RELAIS.getEtatDemain() & 3;
		etatDesLeds.etatCourant.jourNuit = 0;
		if (RELAIS.getJour_Nuit())		//0:jour,3:nuit
			etatDesLeds.etatCourant.jourNuit = 1;  //nuit
		return (etatDesLeds.etat) ;
	}
	void lesLeds::traitementLedsTempo( )
	{
		char  periodeTarifaire[LONGMAXMOT] = { "\0" };
		periodeTarifaire[0] = '\0';
		TINFO.valueGet(&TableauTempoName[TEMPO_UTILISE::ETU_PTEC][0], &periodeTarifaire[0]);
		if ((periodeTarifaire != NULL) && (periodeTarifaire[0] != '\0'))
		{
			if (!strcmp(tableauPtec[0], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_BLEU);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_NUIT);
			}
			else if (!strcmp(tableauPtec[1], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_BLANC);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_NUIT);
			}
			else if (!strcmp(tableauPtec[2], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_ROUGE);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_NUIT);
			}
			else if (!strcmp(tableauPtec[3], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_BLEU);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_JOUR);
			}
			else if (!strcmp(tableauPtec[4], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_BLANC);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_JOUR);
			}
			else if (!strcmp(tableauPtec[5], periodeTarifaire))
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_ROUGE);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_JOUR);
			}
			else
			{
				RELAIS.setAujourdhui(ETAT_JOUR::ETAT_JOUR_INIT);
				RELAIS.setJour_Nuit(ETAT_JOUR::ETAT_JOUR_INIT);
			}
#ifdef PIN_LED_RGB
			traitementLedsRgb( RELAIS.getJour_Nuit(),RELAIS.getAujourdhui());
#endif
		}
	}
	void lesLeds::traitementLedsRgb(ETAT_JOUR jour_nuit, ETAT_JOUR couleur_aujourdhui)
	{
#ifdef PIN_LED_RGB
		 if (jour_nuit== ETAT_JOUR_NUIT)
			 ledRGBON(LED_JOURNUIT, C_NUIT);
		else
			 ledRGBOFF(LED_JOURNUIT);
		if (couleur_aujourdhui == ETAT_JOUR::ETAT_JOUR_BLEU)
		{
			ledRGBON(LES_LEDS_EN_SERIE::LED_BWRJOUR, COULEURS::C_JOUR_BLEU);
		}
		else if (couleur_aujourdhui == ETAT_JOUR::ETAT_JOUR_BLANC)
		{ 
			ledRGBON(LES_LEDS_EN_SERIE::LED_BWRJOUR, COULEURS::C_JOUR_BLANC);
		}
		else if (couleur_aujourdhui == ETAT_JOUR::ETAT_JOUR_ROUGE)
		{
			ledRGBON(LES_LEDS_EN_SERIE::LED_BWRJOUR, COULEURS::C_JOUR_ROUGE);
		}
		else
			ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRJOUR);
#endif
	}
	void lesLeds::traitementLedsTempoDemain()
	{
		char  couleurDemain[LONGMAXMOT] = { "\0" };
		TINFO.valueGet(&TableauTempoName[TEMPO_UTILISE::ETU_DEMAIN][0], &couleurDemain[0]);
		if ((couleurDemain != NULL) && (couleurDemain[0] != '\0'))
		{
			if (!strcmp("BLEU", couleurDemain))
			{
				RELAIS.setEtatDemain(ETAT_JOUR::ETAT_JOUR_BLEU);
				ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_BLEU);
			}
			else if (!strcmp("BLAN", couleurDemain))
			{
				RELAIS.setEtatDemain(ETAT_JOUR::ETAT_JOUR_BLANC);
				ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_BLANC);
			}
			else if (!strcmp("ROUG", couleurDemain))
			{
				RELAIS.setEtatDemain(ETAT_JOUR::ETAT_JOUR_ROUGE);
				ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COULEURS::C_JOUR_ROUGE);
			}
			else
			{
				RELAIS.setEtatDemain(ETAT_JOUR::ETAT_JOUR_INIT);
				ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
			}
		}
	}
void lesLeds::tempoLedOff()
{
#ifdef PIN_LED_RGB
	rgb_ticker.once_ms((unsigned int)BLINK_LED_MS, LedOff,(LES_LEDS_EN_SERIE)PIN_LED_RGB);
#endif
}
void lesLeds::testOn()
{
#ifdef PIN_LED_RGB
	DebuglnF("testOn...");
	ledRGBON(LES_LEDS_EN_SERIE::LED_BWRJOUR, COLOR_BLUE);
	ledRGBON(LES_LEDS_EN_SERIE::LED_BWRDEMAIN, COLOR_ORANGE);
	ledRGBON(LES_LEDS_EN_SERIE::LED_JOURNUIT, COLOR_RED);
#endif
}
void lesLeds::toutesOff()
{
#ifdef PIN_LED_RGB
	DebuglnF("testOff...");
	ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRJOUR);
	ledRGBOFF(LES_LEDS_EN_SERIE::LED_BWRDEMAIN);
	ledRGBOFF(LES_LEDS_EN_SERIE::LED_JOURNUIT);
#endif
}
void lesLeds::initLed(void )
{
#ifdef PIN_LED_RGB
	rgb_led.Begin();
#endif
}
/* ======================================================================
Function: LedOff
Purpose : callback called after led blink delay
Input   : led (defined in term of PIN)
Output  : -
Comments: out of the class for ticker ...
====================================================================== */
void LedOff(LES_LEDS_EN_SERIE led)
{
#ifdef PIN_LED_RGB
	LESLEDS.ledRGBOFF(led);
#endif
}
// Light off the RGB LED
/* ======================================================================
Function: LedRGBON
Purpose : Light RGB Led with HSB value
Input   : Hue (0..255)
Saturation (0..255)
Brightness (0..255)
Output  : -
Comments:
====================================================================== */
void lesLeds::ledRGBON(LES_LEDS_EN_SERIE numLed, unsigned short hue)
{
#ifdef PIN_LED_RGB
		RgbColor target = HslColor(hue / 360.0f, 1.0f, rgb_brightness * 0.005f);
		rgb_led.SetPixelColor(numLed, target);
		rgb_led.Show();
#endif
}
/* ======================================================================
Function: LedRGBOFF
Purpose : light off the RGN LED
Input   : -
Output  : -
Comments: -
====================================================================== */
void lesLeds::ledRGBOFF(LES_LEDS_EN_SERIE numLed)
{
#ifdef PIN_LED_RGB
		rgb_led.SetPixelColor(numLed, RgbColor(0));
		rgb_led.Show();
#endif
}
