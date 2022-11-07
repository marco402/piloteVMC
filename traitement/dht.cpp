
//    FILE: dht.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.10
// PURPOSE: DHT Temperature & Humidity Sensor library for Arduino
//     URL: http://arduino.cc/playground/Main/DHTLib
// inspired by DHT11 library
//
// Released to the public domain
//
//
//
// modified: 07/03/2019 09:12:31
//  by : marc Prieur
//
//Calcul de l’humidité absolue
//https://forum.arduino.cc/t/formule-de-calcul-humidite-absolue/621360/6?pseSrc=pgTutoDht22

#include <Arduino.h>
#ifndef __AVR__
#include "Wifinfo.h"
#endif
#include "mySyslog.h"
#include "constantes.h"
#include "capteur.h"
#include "dht.h"

#define NO_DEBUGDHT
// max timeout is 100usec.
// For a 16Mhz proc that is max 1600 clock cycles
// loops using TIMEOUT use at least 4 clock cycli
// so 100 us takes max 400 loops
// so by dividing F_CPU by 40000 we "fail" as fast as possible

ICACHE_FLASH_ATTR dht::dht(void)
{
}
ICACHE_FLASH_ATTR dht::dht(uint8_t pin,int16_t CorrectionT, int16_t CorrectionH,uint8_t AjustDecimalT,uint8_t AjustDecimalH)
{
	capteur DHT_T(CorrectionT, AjustDecimalT);
	capteur DHT_H(CorrectionH, AjustDecimalH);
	this->pin=pin;
	pinMode(this->pin, INPUT);   //pas de résistance externe
}
//nécessaire si iniatialisation de la config
void ICACHE_FLASH_ATTR dht::initCapteur(uint8_t pin, uint8_t idClasse)
{
	capteur DHT_T();
	capteur DHT_H();
	this->pin = pin;
	pinMode(pin, INPUT);   //pas de résistance externe
	this->idClasse = idClasse;
}
void ICACHE_FLASH_ATTR dht::setCorrectionT(int16_t correction)
{
	DHT_T.setCorrection(correction);
}
void ICACHE_FLASH_ATTR dht::setCorrectionH(int16_t correction)
{
	DHT_H.setCorrection(correction);
}
boolean dht::lectureCapteur(void)
{
uint8_t bits[5];
if(unCycleSurDeux==true)
{
		int16_t capteur;
			int rv = readDHT22(pin, DHTLIB_DHT22_WAKEUP, bits);
			if (rv != DHTLIB_OK )
			{
#ifdef DEBUGDHT
				DebugF("readDHT22: "); Debugln(rv);
#endif
				compteurBuzzer+=1;
				if (compteurBuzzer >= TEMPO_BIP)
				{
					compteurBuzzer = 0;
					DHT_T.setErreur(ERREURS::E_DHTSDB);
					DHT_H.setErreur(ERREURS::E_DHTSDB);  //meme erreur 1 des 2 a la lecture
				}
				return false;		//beep
			}
			else
			{
			capteur = getTemperatureDHT22(bits);
#ifdef DEBUGDHT
			if(idClasse ==2)
				DebuglnF("DHT_T.traiteMesure");
#endif
			DHT_T.traiteMesure(capteur);
			
#ifdef DEBUGDHT
			DebugF("capteur dht T="); Debugln((long)capteur);
			DebugF("cor T: "); Debugln(DHT_T.getCorrection());
#endif
			capteur= getHumiditeDHT22(bits); 
#ifdef DEBUGDHT
			DebugF("capteur dht H="); Debugln((long)capteur);
			DebugF("cor H: "); Debugln(DHT_H.getCorrection());
#endif
			DHT_H.traiteMesure(capteur);
			compteurBuzzer = 0;		//lecture ok
		}
	}
	unCycleSurDeux=!unCycleSurDeux;
	return true;
}

int16_t dht::getTemperatureDHT22(uint8_t bits[]) 
{
	// signe sur msb,valeur en dixième de degré
	return  calculDht22T(bits[2],bits[3]);
}

int16_t dht::calculDht22T(byte Msb,byte Lsb)
{
	int16_t T=  (((Msb & 0x7F)<<8) + Lsb);  // dixième de degrés
    if (Msb & 0x80)
		T *= -1;
	return T;  
}
int16_t dht::calculDht22H(byte Msb,byte Lsb)
{
	int16_t H=((Msb<<8) + Lsb)*0.1;    // dixième de pourcent en pourcent
	return H;
}
int16_t dht::getHumiditeDHT22(uint8_t bits[]) 
{
return calculDht22H(bits[0],bits[1]);;
}

int dht::readDHT22(uint8_t pin, uint8_t  start_time, uint8_t m_data[]) 

{
	uint8_t  i;
	uint16_t cycles[80];
	uint8_t _bit = digitalPinToBitMask(pin);
	uint8_t _port = digitalPinToPort(pin);
	m_data[0] = m_data[1] = m_data[2] = m_data[3] = m_data[4] = 0;

	digitalWrite(pin, HIGH);
	delay(250);
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(2);   //20--->2 datasheet:1ms 

	
	//Make pin input and activate pullup
	digitalWrite(pin, HIGH);
	delayMicroseconds(40);  //datasheet:20 à 40
	pinMode(pin, INPUT_PULLUP);
	delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

	    // First expect a low signal for ~80 microseconds followed by a high signal
	    // for ~80 microseconds again.
		//attente niveau bas time-out à 1000 cycles cpu.  
	    if (expectPulse(LOW,pin, _bit, _port) == 0) {
#ifdef DEBUGDHT
			DebuglnF("Timeout waiting for start signal low pulse.");
#endif
			//sei();
			return DHTLIB_ERROR_TIMEOUT;
	    }
		//attente niveau haut time-out à 1000 cycles cpu.
	    if (expectPulse(HIGH,pin, _bit, _port) == 0) {
#ifdef DEBUGDHT
			DebuglnF("Timeout waiting for start signal high pulse.");
#endif
			//sei();
			return DHTLIB_ERROR_TIMEOUT;
	    }
	    for ( i=0; i<80; i+=2) {
		    cycles[i]   = expectPulse(LOW,pin,_bit,_port);
		    cycles[i+1] = expectPulse(HIGH,pin,_bit,_port);
    }

    for ( i=0; i<40; ++i) {
		uint16_t lowCycles= cycles[2*i];

	    uint16_t highCycles = cycles[2*i+1];
	    if ((lowCycles == 0) || (highCycles == 0)) {
			return DHTLIB_ERROR_TIMEOUT;
	    }

		if(i>0 && !(i % 8))
		    lowCycles-=9;  
	    m_data[i/8] <<= 1;
	    if (highCycles > lowCycles) {
		    m_data[i/8] |= 1;
	    }
    }
    // Check we read 40 bits and that the checksum matches.
    if (m_data[4] == ((m_data[0] + m_data[1] + m_data[2] + m_data[3]) & 0xFF))
		return DHTLIB_OK; 
    else 
	   return DHTLIB_ERROR_CHECKSUM;
}

#ifdef __AVR__
//   https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_pulse.c
uint16_t dht::expectPulse(bool level, uint8_t m_kSensorPin, uint8_t _bit, uint8_t _port)
{
	uint16_t count = 0;
	uint8_t portState = level ? _bit : 0;
	while ((*portInputRegister(_port) & _bit) == portState) {
		if (count++ >= UNEMSENCYCLEHORLOGE) {
			return 0; // Exceeded timeout, fail.
		}
	}
	return count;
}
#else

uint16_t dht::expectPulse(bool level, uint8_t m_kSensorPin, uint8_t _bit, uint8_t _port) {
	uint16_t count = 0;
	unsigned long end = micros() + 1200;
	// On AVR platforms use direct GPIO port access as it's much faster and better
	// for catching pulses that are 10's of microseconds in length:
	while (digitalRead(m_kSensorPin) == level) {
		count++;
		if (micros() > end) {
			return 0; // Exceeded timeout, fail.
		}
	}
	return count;
	}
#endif
void dht::simuMesure(int16_t capteurT,int16_t capteurH)
{
	DHT_T.traiteMesure(capteurT);
	DHT_H.traiteMesure(capteurH);
}
