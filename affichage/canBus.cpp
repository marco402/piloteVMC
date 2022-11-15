// **********************************************************************************
// Programme affichage  arduino pro mini  Pilotage VMC et TEMPO->Gestion du bus CAN liaison traitement affichage
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
//Using library MCP_CAN_lib-master version 0.0.0
//Using library SPI version 1.0
//
// **********************************************************************************

#include <Arduino.h>
#include "dht.h"
#include "constantes.h"
#include "poussoir.h"
#include <mcp_can.h>   //add MCPCAN(modif chercher marc) to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries
#include "canBus.h"
 can_bus::can_bus(unsigned char  freq_can ) : MCP_CAN(PIN_CS_CAN,CLOCKSPI)
 {
	 this->freq_can = freq_can;
 }
 void can_bus::initialiseCanBus(void)
 {
	 if (!initCanBus)
	 {
		 reception.infos = false;
		 reception.locaux = false;
		 reception.distants = false;
		 int nEssai = 10;
		 while ((CAN_OK != begin(MCP_ANY, VITESSE_CAN, freq_can)) && nEssai > 0)              //CAN_500KBPS init can bus : baudrate = 500k
		 {
			 nEssai--;
			 delay(100);
		 }
		 if (nEssai == 0)
		 {
			 Serial.println(F("CAN BUS Shield init fail 10 try"));
			 //initCanBus = false;
			 return;
		 }
		 else
		 {
			 Serial.println(F("CAN BUS Shield init to 100k ok!"));
			 init_Mask(0, 0, 0xFFFFFFF8);                // Init first mask... accept 0 � 7  
			 init_Filt(0, 0, (unsigned char)LES_ID_CAN::ID_MESSAGE_TYPE_0);                // Init first filter...
			 init_Filt(1, 0, (unsigned char)LES_ID_CAN::ID_MESSAGE_TYPE_1);                // Init second filter...
			 init_Filt(2, 0, (unsigned char)LES_ID_CAN::ID_MESSAGE_TYPE_4);                // Init third filter...
			 init_Filt(3, 0, (unsigned char)LES_ID_CAN::ID_MESSAGE_TYPE_5);                // Init four filter...
			 enOneShotTX();
			 setMode(MCP_NORMAL);   //MODE_ONESHOT Change to normal mode to allow messages to be transmitted
			 initCanBus = true;
			 return;
		 }
	 }
	return;
 }
boolean can_bus::traitementReception(void )
{
	unsigned char rxBuf[CAN_MAX_CHAR_IN_MESSAGE];
	unsigned char len = 0; 
	unsigned long canId = 0;
  if(initCanBus)
  {
		 if (CAN_MSGAVAIL == checkReceive())
		 {
			 if (readMsgBuf(&canId, &len, rxBuf) != CAN_OK) 
				 return CAN_NOMSG;
			 if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_0)  //tempo
			 {
				 if (len == MESSAGE_TYPE_0::FIN_MESSAGE_TYPE_0)
				 {
					 reception.heures = rxBuf[MESSAGE_TYPE_0::HEURE];
					 reception.minutes = rxBuf[MESSAGE_TYPE_0::MINUTE];
					 reception.secondes = rxBuf[MESSAGE_TYPE_0::SECONDE];
					 reception.etatLeds = rxBuf[MESSAGE_TYPE_0::LES_LEDS];
					 reception.mode = rxBuf[MESSAGE_TYPE_0::MODE];
					 //Serial.print("reception mode"); Serial.println(reception.mode);
					 POUSSOIR.setLemode((reception.mode));	//sinon on revient sur le mode forcé en fin de tempo
					 reception.etat = rxBuf[MESSAGE_TYPE_0::ETAT];  //libre
					 reception.Rbuzzer=CAN_BUS.getEtResetErreur() | DHTCUISINE.DHT_T.getEtResetErreur() | rxBuf[MESSAGE_TYPE_0::LEBUZZER];
					 //reception.puissanceVMC = rxBuf[MESSAGE_TYPE_0::COURANTVMC];
					 reception.NbMessage += 1;
					 reception.infos = true;
				 }
				 else
				 {
					 Serial.println(F("Problème de longueur message type 0"));
					 erreur = ERREURS::E_CAN_BUS_AFF;
				 }
			 }
			 else if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_1) 
			 {
				 if (len == MESSAGE_TYPE_1::FIN_MESSAGE_TYPE_1)
				 {
					 union CANconv motRecu;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_1::DHT_SDB_T_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_1::DHT_SDB_T_LSB];
					 reception.temperature_sdb_aff = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_1::DHT_SDB_H_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_1::DHT_SDB_H_LSB];
					 reception.humidite_sdb_aff = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_1::TEMP_EXT_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_1::TEMP_EXT_LSB];
					 reception.temperature_ext_aff = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_1::CASAUTO];
					 reception.casAuto = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_1::SEUILAUTO];
					 reception.seuilAuto = motRecu.capteur;
					 reception.NbMessage += 1;
					 reception.distants = true;
				 }
				 else
				 {
					 Serial.println(F("Problème de longueur message type 1"));
					 erreur = ERREURS::E_CAN_BUS_AFF;
				 }
			 }
			 else if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_4) 
			 {
				 if (len == MESSAGE_TYPE_4::FIN_MESSAGE_TYPE_4)
				 {
					 union CANconv motRecu;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_4::AFF_CUISINE_T_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_4::AFF_CUISINE_T_LSB];
					 reception.temperature_cuis_aff = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_4::AFF_CUISINE_H_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_4::AFF_CUISINE_H_LSB];
					 reception.humidite_cuis_aff = motRecu.capteur;
					 reception.etatWifi = rxBuf[MESSAGE_TYPE_4::ETAT_WIFI];
					 reception.luminositeeLeds = rxBuf[MESSAGE_TYPE_4::LUMINOSITE_LEDS_RGB];
#ifdef ALARME
					 reception.alarmeGarage = rxBuf[MESSAGE_TYPE_4::ALARME_GARAGE];
					 reception.alarmePortail = rxBuf[MESSAGE_TYPE_4::ALARME_PORTAIL];
#endif
					 reception.locaux = true;
					 reception.NbMessage += 1;
				 }
				 else
				 {
					 erreur = ERREURS::E_CAN_BUS_AFF;
					 Serial.println(F("Problème de longueur message type 4"));
				 }
			 }
			 else if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_5) 
			 {
				 if (len == MESSAGE_TYPE_5::FIN_MESSAGE_TYPE_5)
				 {
					 union CANconv motRecu;
					 motRecu.capteur = 0;
					 motRecu.b[2] = rxBuf[MESSAGE_TYPE_5::DUREE_FORCAGE_MSB];
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_5::DUREE_FORCAGE_MIDDLE];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_5::DUREE_FORCAGE_LSB];
					 reception.dureeForcageSec = motRecu.capteur;
 					 motRecu.capteur = 0;
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_5::CPT_DELAI_CGT_VITESSE];
					 reception.decompteDelaiCgtVitesse = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[1] = rxBuf[MESSAGE_TYPE_5::NB_MINUTE_JOUR_COURANT_MSB];
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_5::NB_MINUTE_JOUR_COURANT_LSB];
					 reception.NbMinuteActiveJourCourant = motRecu.capteur;
					 motRecu.capteur = 0;
					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_5::MARCHE_ARRET];
					 reception.arret_marche= motRecu.capteur;
					 motRecu.capteur = 0;
//#ifdef TRAITMODE          
 					 motRecu.b[0] = rxBuf[MESSAGE_TYPE_5::FORCAGE_MODE];
					 reception.forcageMode=(MODES) motRecu.capteur;
           //reception.dureeForcage=rxBuf[MESSAGE_TYPE_5::DUREE_FORCAGE]*60;
//#endif
					 reception.NbMessage += 1;
					 reception.infos2 = true;
				 }
			 }
		}	//checkReceive()
	}	//initCanBus
	if (reception.NbMessage == 4)
		return true;
    return false;    // avec ou sans message recu 
}

void can_bus::emission(INT32U id, INT8U len, INT8U *buf)
{
	char sndStat = sendMsgBuf(id, 0, len, buf);
	if (!sndStat == CAN_OK) {
		compteurBuzzer += 1;
		if (compteurBuzzer >= TEMPO_BIP)
		{
			compteurBuzzer = 0;
			erreur = ERREURS::E_CAN_BUS_AFF;
		}
		Serial.print(F("Error Sending Message...")); Serial.println(sndStat);
		compteurErreurConsecutives += 1;
		if (compteurErreurConsecutives > 10)  //100sec pour laisser le temps a traitement de demarrer
		{
			initCanBus = false;
			compteurErreurConsecutives = 0; 
			initialiseCanBus();
		}
	}
	else
		compteurErreurConsecutives = 0;
}

void can_bus::traitementEmissionMESSAGE_TYPE_3(MODES leMode)
{
 	unsigned char buf[MESSAGE_TYPE_3::FIN_MESSAGE_TYPE_3] ;
	buf[MESSAGE_TYPE_3::NOUV_MODE] = leMode;
	//Serial.print("emission mode"); Serial.println(leMode);
	emission( ID_MESSAGE_TYPE_3,MESSAGE_TYPE_3::FIN_MESSAGE_TYPE_3, buf);
}
void can_bus::traitementEmission(uint8_t CuisineTMsb,uint8_t CuisineTLsb,uint8_t CuisineHMsb,uint8_t CuisineHLsb,MODES leMode)
{
  
  if(initCanBus)
  {
	unsigned char buf[MESSAGE_TYPE_2::FIN_MESSAGE_TYPE_2]; 
	buf[MESSAGE_TYPE_2::DHT_CUISINE_T_MSB]=CuisineTMsb;
	buf[MESSAGE_TYPE_2::DHT_CUISINE_T_LSB]=CuisineTLsb;
	buf[MESSAGE_TYPE_2::DHT_CUISINE_H_MSB]=CuisineHMsb;
	buf[MESSAGE_TYPE_2::DHT_CUISINE_H_LSB]=CuisineHLsb;
//#ifdef TRAITMODE
//	buf[MESSAGE_TYPE_2::DECOMPTE_FORCAGE] = reception.decompteTempoArretMarcheForce/60;
//	//VMC.decompteTempoArretMarcheForce = rxBuf[(int)MESSAGE_TYPE_2::DECOMPTE_FORCAGE] * 60;
//#endif
	emission(ID_MESSAGE_TYPE_2,MESSAGE_TYPE_2::FIN_MESSAGE_TYPE_2, buf);
	traitementEmissionMESSAGE_TYPE_3(leMode);        //POUSSOIR.getLeMode() 
	//le mode repasse en AUTO et revient immédiatement en FORCE,
	//probablement au niveau affichage emission d'un message force avant la prise
	//en compte de la réception du message AUTO=>temporiser 5 sec.avant prise en comte d'un nouveau mode
	//DebuglnF("leMode"); Debugln(leMode);   //------------------------>
  }
}
struct_reception can_bus::getStructReception(void) const
{
	return reception;
}
void  can_bus::clearStructReception(void)
{
	reception = {};
}

//uint16_t  can_bus::decDecompteTempoArretMarcheForce(void)
//{
//	if (reception.decompteTempoArretMarcheForce > 0)
//	  reception.decompteTempoArretMarcheForce -= 1;
//  return reception.decompteTempoArretMarcheForce;
//}

uint8_t can_bus::getEtResetErreur(void)
{
	uint8_t temp = erreur;
	erreur = 0;
	return temp;
}
