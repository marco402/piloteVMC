// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Gestion du bus CAN liaison traitement affichage
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


#ifndef CANBUS_H_
#define CANBUS_H_
#include <Arduino.h>
#include "mcp_can.h"
#include "mcp_can_dfs.h"


#define CLOCKSPI 500000		//limitation à 500000 si adaptation 5v/3v3 avec bss138
class can_bus : public MCP_CAN
{
	public:
		can_bus(void);
		void TRAITEMENTEMISSIONCAN(void);
		unsigned char mysendMsgBuf(unsigned long ident, unsigned char ext, unsigned char len,  unsigned char *buf);
		
		void traiteReception(void);
		bool getReceptionCapteurs(void)const;
		void resetReceptionCapteurs(void);
		bool getEtResetReceptionCommandes(void);
		void traiteEmissionCan(unsigned char type, unsigned char heure = 0, unsigned char minute = 0, unsigned char seconde = 0);
		void InitCanBus(unsigned char  freq_can);
		boolean getInitCanBusOK(void);
		uint8_t getEtResetErreur(void);
		int16_t getTCuis(void)const;
		int16_t getHCuis(void)const;
	private:
		boolean initCanBusOK = false;
		boolean receptionCapteurs = false;
		boolean receptionCommandes = false;
		int8_t compteurErreurConsecutives = 0;
		uint8_t erreur=0;
		int16_t tCuis = 0;
		int16_t hCuis = 0;
		uint16_t compteurBuzzer = 0;
};
extern can_bus CAN_BUS;

#endif /* CAN_BUS_H_ */