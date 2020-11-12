// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:ClasssMySql.h
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-08-11 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef HEADER_CLASSMYSQL
#define HEADER_CLASSMYSQL
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <qbasictimer.h>
#include "FormReceptionTempo.h"
class FormReceptionTempo;
using namespace sql;
class ClassMySql : public QObject 
{
Q_OBJECT
protected:
	void timerEvent(QTimerEvent *event);
private:
	struct TeleInfoVmc		//24
	{
		quint32 timestamp;	//4
		//partie tempo
		quint32 compteur;   //4
		quint16 iInstMax;   //2
		quint16 dureeMax;   //2
		uchar etatTempo;    //1
		uchar etatWifi;     //1
		//partie vmc
		qint16 tempExt;    //2
		quint16 tempCuis;   //2
		quint16 tempSdb;    //2
		uchar humCuis;      //1
		uchar humSdb;       //1
		uchar reserve;      //1
		uchar etatVmc;      //1
	};
	FormReceptionTempo *laFormReceptionTempo;
	QBasicTimer timerFinColorDatabase;
	SQLString getSqlString(QString texte);
	void  sqlStop();
	bool messageVersBase(TeleInfoVmc *leMessageVersBase);
	int compteur[6] = { 0, 0, 0, 0, 0, 0 };
	Driver *driver;
	Connection *connection;
	bool connexionOK = false;
	QBasicTimer timerTestDatabase;
	PreparedStatement  *prep_stmt;
public:
	ClassMySql(FormReceptionTempo* parent);
	~ClassMySql();
	bool  initialisationSql(void);
	bool  getCptDernierEnregistrement(void);
	bool traitementMessageS(QByteArray buffer, quint32 *delta);
	bool traitementMessage(QByteArray buffer, quint32 * delta);
	bool testConnexion(void);
	bool connexionIsOK(void) const;
};
#endif
