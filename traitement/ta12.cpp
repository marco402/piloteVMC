// ***************************************************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Mesure et surveillance du courant consommé par le moteur VMC avec un TA12-200.
// ***************************************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
// History : V1.00 2018-03-23 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
 //principe de fonctionnement:
 //lorsque la ventilation est arrêtée, on mesure le courant de repos qu'on déduira de la mesure.
 //pour détecter le maximum de l'onde,à 50 hertz il faut boucler 20ms et garder le maximum
 //la mesure de courant est moyennée sur la durée du cycle VMC.
 //a la lecture cyclique de la mesure ,la mesure moyenne est comparée aux seuils petite ou grande vitesse.
 //Si le mesure est hors tolérance, un bip sonore sera émis toutes les 15 secondes.
#include "Wifinfo.h"
#include <Arduino.h>
#include "constantes.h"
#include "mySyslog.h"
#include "capteur.h"
#include "vmc.h"
#if TEST_CAPTEURS > 0
	#include "adc.h"
#endif
#include "interRelais.h"
#include "ta12.h"
#ifdef ESP8266
#define ADC_REF_VOLT 1							//1v adc reference voltage (see adc_init function for reference)
#else
#define ADC_REF_VOLT 5							//5v adc reference voltage (see adc_init function for reference)
#endif
#define TENSION_MAX 220.0*1.414
#define ADC_RMOYEN 658.0					//800 pour la résistance parallèle 658 calculée
#define ADC_RAPPORT_TA12_200 2000.0		//Transformation ratio 2000*1000 pour ma	
#define ADC_REFRESH 1024.0					//reference resolution used for conversions
#define DUREE_LECTURE_MS 20
const float IN_EN_WATT_TA200 = (ADC_RAPPORT_TA12_200 * TENSION_MAX * ADC_REF_VOLT) / (ADC_REFRESH * ADC_RMOYEN);
int16_t ta12::getSeuilPetiteVitesse(void) const
{
	return ((seuilPetiteVitesse));
}
int16_t ta12::getSeuilGrandeVitesse(void) const
{
	return ((seuilGrandeVitesse));
}
void ta12::lectureZeroCapteur()
{
	zeroCapteur= ta12_lectureTA12();
}
ICACHE_FLASH_ATTR ta12::ta12(void)
{}
ICACHE_FLASH_ATTR ta12::ta12(uint8_t pin,uint8_t ajustDecimal)
{
	this->pin=pin;
	this->ajustDecimal=ajustDecimal;
	pinMode(pin, INPUT);
	this->mesureCycle =0;
}
//nécessaire si iniatialisation de la config
void ICACHE_FLASH_ATTR ta12::initCapteur(uint8_t pin, uint8_t ajustDecimal)
{
	this->pin=pin;
	this->ajustDecimal=ajustDecimal;
	pinMode(pin, INPUT);
	this->mesureCycle =0;
}
//en petite vitesse, le seuil est testé à seuil nominal+-seuil nominal/5
void ta12::setSeuilPetiteVitesse(int16_t seuil)   //esp8266:17 à 24
{
	seuilPetiteVitesse=(seuil);
	seuilPetiteVitesseMin= seuilPetiteVitesse-(seuilPetiteVitesse/20);
	seuilPetiteVitesseMax= seuilPetiteVitesse+(seuilPetiteVitesse/20);
	mesureCycle=0;
}
//en grande vitesse, le seuil est testé à seuil nominal+-seuil nominal/10
void ta12::setSeuilGrandeVitesse(int16_t seuil)
{
	seuilGrandeVitesse=(seuil);
	seuilGrandeVitesseMin= seuilGrandeVitesse-(seuilGrandeVitesse/20);
	seuilGrandeVitesseMax= seuilGrandeVitesse+(seuilGrandeVitesse/20);
	mesureCycle=0;
}
#if TEST_CAPTEURS > 0
void ta12::LectureCapteur(void)
{
	//etalonnage de la consommation
	//il faut la puissance de l'appareil, mesurée ou indiquée sur la notice.
	//TEST_CAPTEURS option de compilation dans ta12.h = 0 en version normale, les versions 1 et 2 pour l'étalonnage.
	//1° TEST_CAPTEURS==1 
	//		relever la valeur brute vmc arrêtée
	//2° TEST_CAPTEURS == 2
	//	affecter la moyenne de cette valeur a zero
	//3° relancer pour afficher les valeurs
			float adc_voltage = 0.0;
#if TEST_CAPTEURS ==1
			int max_voltage = ta12_lectureTA12();
#elif TEST_CAPTEURS == 2
			int16_t zero = 44;
	int max_voltage=ta12_lectureTA12()-zero;
#endif
			adc_voltage = (((double)(max_voltage)) * ADC_REF_VOLT) / ADC_REFRESH;
			float courantMax = adc_voltage / ADC_RMOYEN * ADC_RAPPORT_TA12_200;  //2000 rapport pour le TA12-200    800 pour la résistance parallèle 658 calculée

			//avec 800ohms en parallèle, 1 zener de 5.1V en parallèle,1kohm en série,10kohm à la masse et 1.8kohm en série
			//ampoule à filament	mesure	mV		correction du zero	mesure ma au controleur
			//0						2B		209
			//40W					3C		292		83					185
			//60					43		327		118					250
			//75					4C		371		162					340

			//calcul du facteur de correction moyen->ADC_RMOYEN
			//	83/R*2000000/1.414=185-->x=634
			//	118/R*2000000/1.414=250-->x=667
			//	162/R*2000000/1.414=340-->x=673
			//	Rmoyen->ADC_RMOYEN=658

			float courantMaxEff = courantMax / 1.414;
			float Puissance = courantMaxEff * 220.0;

			#if TEST_CAPTEURS ==1
				Serial.print (max_voltage,DEC);Serial.print ("\t");
				Serial.print (adc_voltage,6);Serial.print ("\t");
				Serial.print (courantMax,6);Serial.print ("\t");
				Serial.print (courantMaxEff,6);	Serial.print ("\t");
				Serial.println (Puissance,6);
			#elif TEST_CAPTEURS == 2
				Serial.print (Puissance,6);Serial.print ("\t");
			#endif
}
#else
void ta12::lectureCapteur(void)
{
	//mPuissanceVMC = String.Format("{0:0.00}", Conversion.Val(value) * ADC_REF_VOLT / ADC_REFRES / ADC_RMOYEN * ADC_RAPPORT_TA12_200 / 1.414 * TENSION_SECTEUR / 1000.0)
	//TEST_CAPTEURS option de compilation dans ta12.h = 0 en version normale, les versions 1 et 2 pour l'étalonnage.

	int16_t capteur = (int16_t)(ta12_lectureTA12() - zeroCapteur);
	traiteMesure(capteur);
}
#endif
uint16_t ta12::ta12_lectureTA12(void)
{
	unsigned long fin = millis() + DUREE_LECTURE_MS;
	int16_t  courant_max = 0;
	while(millis()<fin)
	{
		int courant = analogRead(pin);   //maxi=1V en entrée

		if (courant>courant_max)
		{
			courant_max=courant;
		}
	}
	//DebugF("Ivmc="); Debugln(courant_max);
	return courant_max;
}
boolean ta12::traitementLectureTa12(void)
{
	if (RELAIS.getEtatReelRelaisMarcheArret() == ARRET_MARCHE::MARCHE_REL)
	{
		lectureCapteur();
		//return true;
		//return(traitementSeuilTa12());	//true si ok
	}
	else
	{ 
		lectureZeroCapteur();
		traiteMesure(0);
	}
	return true;
}
boolean ta12::traitementSeuilTa12(void)
{
	static uint16_t compteurBuzzer = 0;
	int SeuilMin=seuilGrandeVitesseMin;
	int SeuilMax=seuilGrandeVitesseMax;
	if (RELAIS.getEtatReelRelaisMarcheArret()== ARRET_MARCHE::MARCHE_REL)
	{
		if(RELAIS.getEtatRelaisVitesse()== VITESSE_RELAIS::LENT_REL)
		{
			SeuilMin= seuilPetiteVitesseMin;
			SeuilMax= seuilPetiteVitesseMax;
		}
		if (SeuilMin>0) 
		{
			if ((getMoyennePeriode() < SeuilMin) || (getMoyennePeriode() > SeuilMax))   //mesureCycle
			{
				compteurBuzzer+=1;
				if(compteurBuzzer>=TEMPO_BIP)
				{
					compteurBuzzer=0;
					erreur = ERREURS::E_TA12;
				}
				return false;
			}
			else
			{
				compteurBuzzer=0;
				return true;
			}
		}
	}
	return true;
}
uint8_t ta12::getPuissanceConsommee(void) const
{
	return (uint8_t)(getMoyennePeriodeCourante()*IN_EN_WATT_TA200); // 25 et 40    getMesureCycle()
}
