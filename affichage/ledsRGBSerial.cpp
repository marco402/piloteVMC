// **********************************************************************************
// Programme affichage  arduino pro mini  Pilotage VMC et TEMPO->Pilotage de leds RGB s�rie type APA106
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
//Using library NeoPixelBus-master version 2.3.4
//
// **********************************************************************************
#include <Arduino.h>
#include "constantes.h"
#include <NeoPixelBrightnessBus.h>
#include "canBus.h"
#include "ledsRGBSerial.h"
#ifdef PIN_LED_RGB
	#ifdef __AVR__
	#include <avr/power.h>
	#endif
NeoPixelBrightnessBus<NeoRgbFeature, Neo800KbpsMethod>  rgb_led(NB_LEDS_RGB, PIN_LED_RGB);
#endif
//https://www.rapidtables.com/web/color/RGB_Color.html
RgbColor green(0,colorSaturation, 0);
RgbColor red( colorSaturation,0, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor yellow(colorSaturation,colorSaturation,0);
RgbColor orange(colorSaturation,85,0);
RgbColor gris(150, 150, 150);
RgbColor black(0, 0, 0);
ledsRgbSerial::ledsRgbSerial(void)
{
}
void ledsRgbSerial::init(void)
{
	rgb_led.SetBrightness(brightness);
	traitementLedsRGB(COULEUR_JOUR::COULEUR_JOUR_INIT, LES_LEDS_RGB_LED_JOUR);
	traitementLedsRGB(COULEUR_JOUR::COULEUR_JOUR_INIT, LES_LEDS_RGB_LED_DEMAIN);
    //traitementLedsRGB(COULEUR_JOUR::COULEUR_JOUR_INIT, LES_LEDS_RGB_LED_JOUR_NUIT);
	rgb_led.Show();
}

int memoEtatDesLeds = -1;
int memoLuminositeeLeds = brightness;
void ledsRgbSerial::traitement(struct_reception reception)
{
	if(reception.infos)
	{	
		if(reception.luminositeeLeds != memoLuminositeeLeds)
		{
			  rgb_led.SetBrightness(reception.luminositeeLeds);
			  Serial.print("rgb_led.SetBrightness "); Serial.println(reception.luminositeeLeds);  //a voir prise en compte uniquement au reboot???
			  memoLuminositeeLeds = reception.luminositeeLeds;
			  rgb_led.Show();                                                                     //voir s'il faut .show
		}        
		if(reception.etatLeds != memoEtatDesLeds)
		{ 
		  union leds etatDesLeds;
		  //   if(reception.etatLeds!=12)
		  //      Serial.print("reception.etatLeds "); Serial.println(reception.etatLeds);
		  etatDesLeds.etat=reception.etatLeds;
		  traitementLedsRGB(etatDesLeds.etatCourant.demain,LES_LEDS_RGB_LED_DEMAIN);
		  traitementLedsRGB(etatDesLeds.etatCourant.aujourdhui,LES_LEDS_RGB_LED_JOUR);
		  //traitementLedsRGB(etatDesLeds.etatCourant.jourNuit,LES_LEDS_RGB_LED_JOUR_NUIT);
		  memoEtatDesLeds=reception.etatLeds ; 
		}
	}
}
void ledsRgbSerial::traitementLedsRGB(unsigned char etat,LES_LEDS_RGB indiceLedRGB)
{
switch  (etat)
	{
	case COULEUR_JOUR::COULEUR_JOUR_BLEU:
		rgb_led.SetPixelColor(indiceLedRGB, blue);
		break;
	case COULEUR_JOUR::COULEUR_JOUR_BLANC:
		rgb_led.SetPixelColor(indiceLedRGB, white);
		break;
	case COULEUR_JOUR::COULEUR_JOUR_ROUGE:
		rgb_led.SetPixelColor(indiceLedRGB, red);
		break;
	case COULEUR_JOUR::COULEUR_JOUR_INIT:
		rgb_led.SetPixelColor(indiceLedRGB, green);  //gris donne blanc
	}
	rgb_led.Show();
}
