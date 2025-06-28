//
//    FILE: dht.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.10
// PURPOSE: DHT Temperature & Humidity Sensor library for Arduino
//     URL: http://arduino.cc/playground/Main/DHTLib
//
// HISTORY:
// see dht.cpp file
//
//
//
// modified: 07/03/2019 09:12:31
//  Author: marc Prieur
//
#ifndef DHT_H__
#define DHT_H__
#include <Arduino.h>
#include "capteur.h"

#define TIMEOUT (F_CPU/40000)
#define MAXTIMINGS 85
#define DHT_LIB_VERSION "0.1.10"

#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_ERROR_CHECKSUM_OU_TIMEOUT    -3
#define DHTLIB_INVALID_VALUE    -999
#define DHTLIB_INVALID_VALUE_LECTURE    0xff
#define DHTLIB_INVALID_VALUE_CRC    0xfe
#define DHTLIB_DHT11_WAKEUP     18
#define DHTLIB_DHT22_WAKEUP     1
#define ONE_DURATION_THRESH_US 30

//static const uint8_t   DEF_MOY_H = 70;   //mettre des seuils assez haut pour l'init, capteur->traiteMesure prend  en compte au depart que les cycles sous ce seuil.

typedef enum {TEMPERATURE=0,HUMIDITE}DHT_TouH;

class dht 
{
public:
	dht(void);
	dht(uint8_t pin, int16_t CorrectionT, int16_t CorrectionH, uint8_t AjustDecimalT, uint8_t AjustDecimalH);
	boolean lectureCapteur(void);
	//void initCapteur(uint8_t pin, int16_t CorrectionT, int16_t CorrectionH, uint8_t AjustDecimalT, uint8_t AjustDecimalH);
	void initCapteur(uint8_t pin,uint8_t idClasse);
	void setCorrectionT(int16_t Correction);
	void setCorrectionH(int16_t Correction);
	void simuMesure(int16_t capteurT,int16_t capteurH);
	capteur DHT_T;
	capteur DHT_H;
private:
	uint8_t pin;

	int16_t getTemperatureDHT22(uint8_t bits[]) ;
	int16_t getHumiditeDHT22(uint8_t bits[]) ;
	int readDHT22(uint8_t pin, uint8_t wakeupDelay,uint8_t  bits[]) ;
	int16_t calculDht22T(byte Msb,byte Lsb);
	int16_t calculDht22H(byte Msb,byte Lsb);
	uint16_t expectPulse(bool level,uint8_t m_kSensorPin,uint8_t _bit,uint8_t _port);
	uint8_t idClasse;
	boolean unCycleSurDeux = true;
	uint16_t compteurBuzzer = 0;
};
extern	dht DHTSDB;		  //pour traitement
extern	dht DHTCUISINE;   //pour affichage


#endif
//
// END OF FILE
//
