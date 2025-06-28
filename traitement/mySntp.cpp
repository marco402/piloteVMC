#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "mySyslog.h"
#include "myWifi.h"
#include "SNTPTime.h"
#include "SNTPClock.h"
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
//ajoute 300 e cbLocalTimeStruct											*
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
//si m_daylight ajoute 0 ou 3600 suivant ete hiver							*
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
 mySntp::mySntp() //ICACHE_FLASH_ATTR
{
	sntp_set_timezone(TIMEZONE);
}
 mySntp::~mySntp()    //ICACHE_FLASH_ATTR
{
}
void  mySntp::init(void)   //ICACHE_FLASH_ATTR
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
	//memorise m_timezone=3600 et m_daylight=1								*
	//interroge le serveur "de.pool.ntp.org"--> m_secsSince1900				*
	//attache secondTicker interval 1 seconde								*
	//retourne l'heure tu 1970												*
	//le serveur sera de nouveau interroge toutes les heures par defaut,	*
	//***********************************************************************
	//--daylight=1: prise en compte heure d'ete(+3600) heure d'hiver(+0)
	Clock.begin("de.pool.ntp.org", 3600, 1);  //3600 decalage Paris / TU (heure d'hiver) 1 heure d'ete
	ClockCbFunction();
}
/* ======================================================================
Function: getCycle1Seconde
	Purpose : test si seconde +1
	Input : -
	Output : true si seconde suivante
	Comments : -
	====================================================================== */
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
void  mySntp::clrMinuit(void) {      //ICACHE_FLASH_ATTR
	minuit = false;
}
/* ======================================================================
Function: TestSiMinuit
Purpose : test si minuit
	-passe reste true la premiere minute
	-minuit reste true jusqu'a l'appel de clrMinuit lors de la prise en compte de minuit(1 seul appel sinon implementer un compteur)
Input   : -
Output  : -
Comments: -
====================================================================== */
bool passe = false;
ulong memoSecondesToMidnight = 0;  //protection contre la detection de minuit quelques cycles par jour ?
bool mySntp::TestSiMinuit()
{
	//static bool passe = false;
	if ((Clock.getHour() == 0 && Clock.getMinute() == 0 && Clock.getSecond() >= 0) && !passe)   //minuit au cas ou on rate 1 seconde
	{
		minuit = true;
		passe = true;
	}
	else if (Clock.getMinute() > 0)
		passe = false;

	//if ((Clock.getHour() == 0 && Clock.getMinute() == 0 && Clock.getSecond() >= 0) && !passe)   // au cas ou on rate 1 seconde
	//{
	//	ulong t = Clock.getTimeSeconds();
	//	if (memoSecondesToMidnight > 0)
	//	{
	//		if (!(((t - memoSecondesToMidnight) < 86340) || ((t - memoSecondesToMidnight) > 86460)))
	//		{
	//		minuit = true;
	//		passe = true;
	//		memoSecondesToMidnight = t;
	//		}
	//	}
	//	else  //cycle d'initialisation
	//	{
	//		minuit = true;
	//		passe = true;
	//		memoSecondesToMidnight = t;
	//	}
	//}
	//if (Clock.getMinute()>0)
	//	passe = false;
	return minuit;
}
