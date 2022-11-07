// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Pilotage de leds RGB série type APA106
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
#ifndef LEDSRGBSERIAL_H
#define LEDSRGBSERIAL_H
#define colorSaturation 255
#define  brightness 5			//150 130 100 50
class ledsRgbSerial
{
public:
	ledsRgbSerial(void);
	void init(void);
	void traitement(struct_reception reception);
private:
	void traitementLedsRGB(unsigned char etat,LES_LEDS_RGB indiceLedRGB);
};
extern ledsRgbSerial LEDS_RGB_SERIAL;















//les leds chauffage et cumulus sont branch�es sur le deuxi�me contact des relais sinon laisser en test




//#include "C:\Program Files (x86)\Arduino\libraries\LibTeleinfo-syslog\examples\Wifinfo\config.h"

#endif
