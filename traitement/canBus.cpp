// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Gestion du bus CAN liaison traitement affichage
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
//Using library MCP_CAN_lib-master version 0.0.0
//Using library SPI version 1.0
//
//I changed mcpcan to set spi clock for interface 5V-3.3V bss138 <1000000hz
// **********for mcp_can.h:**************
//class MCP_CAN
//{
//private:
//	uint32_t m_clockSPI;								//ajout clockSPI
//public:
//	MCP_CAN(INT8U _CS, uint32_t clockSPI);				//ajout clockSPI 
//***********for mcp_can.cpp:*************
	//MCP_CAN::MCP_CAN(INT8U _CS, uint32_t clockSPI)	//marc ajout clockSPI
	//{
	//	m_clockSPI = clockSPI;							//marc ajout clockSPI
//
// **********************************************************************************
#include <Arduino.h>
#include "Wifinfo.h"
#include "mcp_can.h"
#include "mySyslog.h"
#include "dht.h"
#include "capteur.h"
#include "vmc.h"
#include "ledsRGBSerial.h"
#include "ta12.h"
#include "smt160.h"
#include "buzzer.h"
#include "myWifi.h"
#include "interRelais.h"
#include "myTinfo.h"
#include "mySntp.h"
#include "config.h"
#include "canBus.h"
#ifdef ALARME
  #include "alarme.h"
#endif
#ifdef PIN_CS_CAN
ICACHE_FLASH_ATTR can_bus::can_bus(void) : MCP_CAN(PIN_CS_CAN, CLOCKSPI)
{
}
#endif
void can_bus::TRAITEMENTEMISSIONCAN(void)
{
	if (!initCanBusOK)
		return;
	//DebugF("mcp2515_readStatus():"); Debugln(mcp2515_readStatus());
	//DebugF("errorCountRX:"); Debugln(errorCountRX());
	//DebugF("errorCountTX:"); Debugln(errorCountTX());
	//DebugF("getError:"); Debugln(getError());
	//DebugF("compteurErreurConsecutives:"); Debugln(compteurErreurConsecutives);
	if (compteurErreurConsecutives == 40)  //10sec
	{
		initCanBusOK = false;
		compteurErreurConsecutives = 0;
		InitCanBus(MCP_16MHZ);
	}
	traiteEmissionCan((unsigned char)ID_MESSAGE_TYPE_0, Clock.getHour(), Clock.getMinute(), Clock.getSecond()); //90ms.
	traiteEmissionCan((unsigned char)ID_MESSAGE_TYPE_1);
	traiteEmissionCan((unsigned char)ID_MESSAGE_TYPE_4);
	traiteEmissionCan((unsigned char)ID_MESSAGE_TYPE_5);
}
 void ICACHE_FLASH_ATTR can_bus::InitCanBus(unsigned char freq_can)
 {
	  int nEssai = 10;
	 while ((CAN_OK != begin(MCP_ANY, VITESSE_CAN,freq_can )) && nEssai > 0)              //ok a 100 pb � CAN_500KBPS   MCP_16MHZ(vitesse du quartz sur la carte can a confirmer)
	 {
		 nEssai--;
		 delay(100);
	 }
	 if (nEssai == 0)
	 {
		 DebuglnF("CAN BUS Shield init fail 10 try");
		 initCanBusOK= false;
		 return;
	 }
	 else
	 {
		 init_Mask(0, 0, 0xFFFFFFFC);                // Init first mask... accept 0 � 7  
		 init_Filt(0, 0, (unsigned char)ID_MESSAGE_TYPE_2);                // Init first filter...
		 init_Filt(1, 0, (unsigned char)ID_MESSAGE_TYPE_3);                // Init second filter...
		 enOneShotTX();
		 setMode(MCP_NORMAL);   // loopback par defaut
		DebuglnF("CAN BUS Shield init (speed SPI:500k,speed CAN:250k,freq CAN 16Mhz) ok!");
		initCanBusOK = true;
		return;
	 }
 }
  boolean can_bus::getInitCanBusOK(void)
 {
	 return initCanBusOK;
 }
 //charge les 2 capteurs cuisine et/ou le mode en cours ou a changer et positionne receptionCapteurs et/ou receptionCommandes
 void can_bus::traiteReception(void )
{
	if (!initCanBusOK)
		return;
	for (int mes = 0; mes < 2; mes++)
	{
		if (CAN_MSGAVAIL == checkReceive())   //pas it pour r�cup�rer une IO   if (!digitalRead(PIN_INT_SPI))                         // If CAN0_INT pin is low, read receive buffer
		{
			unsigned long canId = 0;
			unsigned char len = 0;
			unsigned char rxBuf[CAN_MAX_CHAR_IN_MESSAGE];
			if (readMsgBuf(&canId, &len, rxBuf) != CAN_OK)      // Read data: len = data length, buf = data byte(s)
				return; 
			if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_2)  //tempo
			{
				if (len == FIN_MESSAGE_TYPE_2)
				{
					union CANconv motRecu;
					motRecu.capteur = 0;
					motRecu.b[1] = rxBuf[(int)MESSAGE_TYPE_2::DHT_CUISINE_T_MSB];
					motRecu.b[0] = rxBuf[(int)MESSAGE_TYPE_2::DHT_CUISINE_T_LSB];
					//DHTCUISINE_T.traiteMesure(motRecu.capteur);
					tCuis = motRecu.capteur;						//transfert des donn�es en m�me temps que lectureCapteurs
					motRecu.capteur = 0;
					motRecu.b[1] = rxBuf[(int)MESSAGE_TYPE_2::DHT_CUISINE_H_MSB];
					motRecu.b[0] = rxBuf[(int)MESSAGE_TYPE_2::DHT_CUISINE_H_LSB];
					//DHTCUISINE_H.traiteMesure(motRecu.capteur);
					hCuis = motRecu.capteur;						//transfert des donn�es en m�me temps que lectureCapteurs
					//DebugF("rec:DHTCUISINE_H: "); Debugln(motRecu.capteur);
					receptionCapteurs = true;
				}
				else
				{
					erreur = ERREURS::E_CAN_BUS_TRAIT;
					DebuglnF("Probleme de longueur message type 2");
					//Serial.println("Probleme de longueur message type 2");
				 }
			}
			else  if (canId == LES_ID_CAN::ID_MESSAGE_TYPE_3)  //tempo
			{
				if (len == FIN_MESSAGE_TYPE_3)
				{
					VMC.setLeMode((MODES)rxBuf[MESSAGE_TYPE_3::NOUV_MODE]);
					//DebugF("Receive mode:"); Debugln((MODES)rxBuf[MESSAGE_TYPE_3::NOUV_MODE]);
					receptionCommandes = true;
				}
				else
				{ 
					erreur = ERREURS::E_CAN_BUS_TRAIT;
					DebuglnF("Probleme de longueur message type 3");
					//Serial.println("Probleme de longueur message type 3");
				}
			}
		}
	}
}
unsigned char  can_bus::mysendMsgBuf(unsigned long ident, unsigned char ext, unsigned char len,  unsigned char *buf)
{
	return sendMsgBuf(ident, ext, len, buf);
}
//type=0:emission du temps,du mode, de l'�tat courant et du buzzer, type=1:emission de la temp�rature et humidit� SDB et temp�rature exterieur.
void can_bus::traiteEmissionCan(unsigned char type, unsigned char heure, unsigned char minute, unsigned char seconde)
	{
	byte sndStat = 0;
		if (type== ID_MESSAGE_TYPE_0)
		{ 
		unsigned char buf[MESSAGE_TYPE_0::FIN_MESSAGE_TYPE_0];
		//max 8 byte pour can
		buf[MESSAGE_TYPE_0::HEURE] = heure;
		buf[MESSAGE_TYPE_0::MINUTE] = minute;
		buf[MESSAGE_TYPE_0::SECONDE] =seconde;
		buf[MESSAGE_TYPE_0::LES_LEDS] =LESLEDS.getEtatLeds();
		//DebugF("lesleds"); Debugln(buf[MESSAGE_TYPE_0::LES_LEDS] );
		buf[MESSAGE_TYPE_0::MODE] = VMC.getLeMode();
		buf[MESSAGE_TYPE_0::ETAT] =0;  //libre
		//buf[MESSAGE_TYPE_0::LEBUZZER] =BUZZER.getBuzzer();  //true->beep
		buf[MESSAGE_TYPE_0::LEBUZZER] = TEMPEXT.getEtResetErreur()| DHTSDB.DHT_T.getEtResetErreur() | CAPTEURIINST.getEtResetErreur()| TA12.getEtResetErreur()| CAN_BUS.getEtResetErreur()| MYTINFO.getEtResetErreur();  //true->beep
		BUZZER.clrBuzzer();
		buf[MESSAGE_TYPE_0::COURANTVMC] =TA12.getPuissanceConsommee();
		 sndStat = sendMsgBuf(LES_ID_CAN::ID_MESSAGE_TYPE_0, 0, MESSAGE_TYPE_0::FIN_MESSAGE_TYPE_0, buf);
		//DebugF("Send mode:"); Debugln(buf[MESSAGE_TYPE_0::MODE]);
	}
	else if (type== ID_MESSAGE_TYPE_1)
	{
		unsigned char buf[MESSAGE_TYPE_1::FIN_MESSAGE_TYPE_1];
		buf[MESSAGE_TYPE_1::DHT_SDB_T_MSB] =  DHTSDB.DHT_T.getMesureCycleMsb();
		buf[MESSAGE_TYPE_1::DHT_SDB_T_LSB] = DHTSDB.DHT_T.getMesureCycleLsb();
		buf[MESSAGE_TYPE_1::DHT_SDB_H_MSB] = DHTSDB.DHT_H.getMesureCycleMsb();
		buf[MESSAGE_TYPE_1::DHT_SDB_H_LSB] = DHTSDB.DHT_H.getMesureCycleLsb();
		buf[MESSAGE_TYPE_1::TEMP_EXT_MSB] = TEMPEXT.getMesureCycleMsb();
		buf[MESSAGE_TYPE_1::TEMP_EXT_LSB] =TEMPEXT.getMesureCycleLsb();
		buf[MESSAGE_TYPE_1::SEUILAUTO] =VMC.getSeuilAuto();
		buf[MESSAGE_TYPE_1::CASAUTO] =VMC.getCasAuto();

		 sndStat = sendMsgBuf(LES_ID_CAN::ID_MESSAGE_TYPE_1, 0, MESSAGE_TYPE_1::FIN_MESSAGE_TYPE_1, buf);
	}
	else if (type == ID_MESSAGE_TYPE_4)
		{
			//DebugF("em:DHTCUISINE_H: "); Debugln(DHTCUISINE_H.getMesureCycle());
			unsigned char buf[MESSAGE_TYPE_4::FIN_MESSAGE_TYPE_4];
			buf[MESSAGE_TYPE_4::AFF_CUISINE_T_MSB] = DHTCUISINE_T.getMesureCycleMsb();
			buf[MESSAGE_TYPE_4::AFF_CUISINE_T_LSB] = DHTCUISINE_T.getMesureCycleLsb();
			buf[MESSAGE_TYPE_4::AFF_CUISINE_H_MSB] = DHTCUISINE_H.getMesureCycleMsb() ;
			buf[MESSAGE_TYPE_4::AFF_CUISINE_H_LSB] = DHTCUISINE_H.getMesureCycleLsb();
			buf[MESSAGE_TYPE_4::ETAT_WIFI] = WIFI.getWifiUser() | (MYTINFO.getEtatWifi() << 1);
			buf[MESSAGE_TYPE_4::LUMINOSITE_LEDS_RGB] = CONFIGURATION.config.tempo.luminositeeLedsRgb;
			buf[MESSAGE_TYPE_4::ALARME_GARAGE] = MYALARME.getEtatAlarmeGarage();
			sndStat = sendMsgBuf(LES_ID_CAN::ID_MESSAGE_TYPE_4, 0, MESSAGE_TYPE_4::FIN_MESSAGE_TYPE_4, buf);
		}
	else if (type == ID_MESSAGE_TYPE_5)
		{
			unsigned char buf[MESSAGE_TYPE_5::FIN_MESSAGE_TYPE_5];
			buf[MESSAGE_TYPE_5::DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MSB] = VMC.getDecompteTempoArretMarcheForce()>> 16;
			buf[MESSAGE_TYPE_5::DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MIDDLE] = VMC.getDecompteTempoArretMarcheForce()>> 8;
			buf[MESSAGE_TYPE_5::DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_LSB] = VMC.getDecompteTempoArretMarcheForce() & 0xFF;
			buf[MESSAGE_TYPE_5::CPT_DELAI_CGT_VITESSE] = RELAIS.getDecompteDelaiCgtVitesse();
			buf[MESSAGE_TYPE_5::NB_MINUTE_JOUR_COURANT_MSB] = RELAIS.getNbMinuteActiveJourCourant()>> 8;
			buf[MESSAGE_TYPE_5::NB_MINUTE_JOUR_COURANT_LSB] = RELAIS.getNbMinuteActiveJourCourant() & 0xFF;
			buf[MESSAGE_TYPE_5::MARCHE_ARRET] = RELAIS.getEtatReelRelaisMarcheArret() & 0xFF;
#ifdef TRAITMODE
			buf[MESSAGE_TYPE_5::FORCAGE_MODE] = VMC.getForcageMode();
#endif
			sndStat = sendMsgBuf(LES_ID_CAN::ID_MESSAGE_TYPE_5, 0, MESSAGE_TYPE_5::FIN_MESSAGE_TYPE_5, buf);
		}
	if (sndStat != CAN_OK) {
		compteurBuzzer += 1;
		if (compteurBuzzer >= TEMPO_BIP)
		{
			compteurBuzzer = 0;
			erreur = ERREURS::E_CAN_BUS_TRAIT;
		}
		DebugF("Error Sending Message..."); Debugln(sndStat);
		compteurErreurConsecutives += 1;
	}
}
bool can_bus::getReceptionCapteurs(void)const
{
	return receptionCapteurs;
}
void can_bus::resetReceptionCapteurs(void)
{
	receptionCapteurs =false;
}
bool can_bus::getEtResetReceptionCommandes(void)
{
	bool temp = receptionCommandes;
	receptionCommandes =false;
	return temp;
}
uint8_t can_bus::getEtResetErreur(void)
{
	uint8_t temp = erreur;
	erreur = 0;
	return temp;
}
int16_t can_bus::getTCuis(void)const
{
	return tCuis;
}
int16_t can_bus::getHCuis(void)const
{
	return hCuis;
}
