#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "mySyslog.h"
#include "myWifi.h"
#include <SNTPTime.h>
#include <SNTPClock.h>
#include "mySntp.h"
//********************ATTENTION CORRECTIF DANS SNTPTIME.CPP*********************************
//ligne 142 if (t.tm_mday - t.tm_wday >= 25 && (t.tm_wday || (t.tm_hour >= 2)))  //marc add two brackets
extern "C" {
#include <sntp.h>
}
Ticker Every_1_Sec;
uint32 last_second = 0;
const sint8 TIMEZONE = 0;   //heure TU
//*************************ClockCbFunction***********************************
//copy de Clock.getTimeStruct dans cbLocalTimeStruct						*
//Clock.getTimeStruct=localtime=f(heure tu 1970,m_timezone et m_daylight)	*
//ajoute 300 à cbLocalTimeStruct											*
//attachCb ClockCbFunction avec ce temps									*
//**************************attachCb*****************************************
// m_cbLocalTimeSec =cbLocalTimeStruct-m_timezone							*
// m_cbLocalTimeSec = m_cbLocalTimeSec-m_daylight							*
// m_callback = callback pour secondTick(1sec)								*
//*****************************secondTick************************************
//attache secondTicker(1sec)												*
//lecture du temps 1900->sntp_get_current_timestamp()						*
//affichage des 3 messages de debug											*
//m_localTimeStruct=f(heure tu 1970,m_timezone et m_daylight)				*
//if heure 1970>=m_cbLocalTimeSec--->m_callback()=ClockCbFunction			*
//***************************localtime***************************************
//heure1900+timezone														*
//si m_daylight ajoute 0 ou 3600 suivant été hiver							*
//retourne la structure localTime
//***************************************************************************
//10 heures
#define SNTP_UPDATE_DELAY           36000000

void ClockCbFunction()
{
	tm cbLocalTimeStruct;
	memcpy((void*)&cbLocalTimeStruct, (const void*)Clock.getTimeStruct(), sizeof(tm));
	//add some time and recalculate cbLocalTimeStruct
	addTimeSeconds(&cbLocalTimeStruct, 300UL); //??
	Clock.attachCb(&cbLocalTimeStruct, (SNTPClock::callback_t)ClockCbFunction);
}
ICACHE_FLASH_ATTR mySntp::mySntp()
{
	sntp_set_timezone(TIMEZONE);
}
ICACHE_FLASH_ATTR mySntp::~mySntp()
{
}
void ICACHE_FLASH_ATTR mySntp::init(void)
{
	//*********************sntp_init()*******************************
	//lance un premier request sur pool.ntp.org						*
	//																*
	//***************************************************************
	sntp_init();
	sntp_setservername(0, (char*)"de.pool.ntp.org");
	sntp_setservername(1, (char*)"time.windows.com");
	sntp_setservername(2, (char*)"time.nist.gov");

	//**************************Clock.begin**********************************
	//mémorise m_timezone=3600 et m_daylight=1								*
	//interroge le serveur "de.pool.ntp.org"--> m_secsSince1900				*
	//attache secondTicker interval 1 seconde								*
	//retourne l'heure tu 1970												*
	//le serveur sera de nouveau interrogé toutes les heures par défaut,	*
	//***********************************************************************
	//--daylight=1: prise en compte heure d'été(+3600) heure d'hiver(+0)
	Clock.begin("de.pool.ntp.org", 3600, 1);  //3600 décalage Paris / TU (heure d'hiver) 1 heure d'été
	ClockCbFunction();
}
boolean mySntp::getCycle1Seconde(void)
{
	uint32 this_second = Clock.getSecond();
	if (this_second != last_second)
	{
		last_second = this_second;
		return true;
	}
	return false;
}
bool mySntp::getMinuit(void) const {
		return minuit;
}
void ICACHE_FLASH_ATTR mySntp::clrMinuit(void) {
	minuit = false;
}
/* ======================================================================
Function: TestSiMinuit
Purpose : test si minuit
Input   : true if first call
		  true if needed to print on serial debug
Output  : -
Comments: -
====================================================================== */
void mySntp::TestSiMinuit()
{
	static bool passe = false;
	if ((Clock.getHour() == 0 && Clock.getMinute() == 0 && Clock.getSecond() >= 0) && !passe)   //minuit au cas ou on rate 1 seconde
	{
		minuit = true;
		passe = true;
	}
	else if (Clock.getMinute()>0)
		passe = false;
}
