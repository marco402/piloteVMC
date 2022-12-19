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
//
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
#include "constantes.h"
#include <mcp_can.h>                 //si erreur compile, verifier outils type de carte---->il faut arduino mini atmega 328P
#define xxTESTCAN
#define CLOCKSPI 500000			//limitation � 500000 si adaptation 5v/3v3 avec bss138
class can_bus : public MCP_CAN
{
	public:
		can_bus(unsigned char  freq_can);
		void initialiseCanBus(void);
		boolean traitementReception(void);
		void traitementEmission(uint8_t CuisineTMsb,uint8_t CuisineTLsb,uint8_t CuisineHMsb,uint8_t CuisineHLsb,MODES leMode);
		void clearStructReception(void);
		struct_reception getStructReception(void) const;
		//uint16_t  decDecompteTempoArretMarcheForce(void);
		uint8_t getEtResetErreur(void);
	private:
		void traitementEmissionMESSAGE_TYPE_3(MODES leMode);
		void emission(INT32U id, INT8U len, INT8U *buf);
		boolean initCanBus;
		struct_reception reception;
		unsigned char freq_can;
		uint8_t erreur = 0;
		uint16_t compteurBuzzer = 0;
		int8_t compteurErreurConsecutivesEmission = 0;
		int8_t compteurErreurConsecutivesReception = 0;
		int8_t memoSecondes = 0;
#ifdef TESTCAN
		uint16_t cptTestInitCan = 0;
#endif
};
extern can_bus CAN_BUS;
#endif /* CAN_BUS_H_ */
