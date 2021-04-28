// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:ClassReceptionUdp.h
// Written by Marc Prieur (https://marco40github.wixsite.com/website/pilotevmc)(marco40_github@sfr.fr)
//
//History : V1.00 2019-07-28 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef HEADER_CLASSTHRECEPTIONUDP
#define HEADER_CLASSTHRECEPTIONUDP
#include <cppconn\driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <qbasictimer.h>
#include <QtNetwork>
#include "FormReceptionTempo.h"
#include "ClassMySql.h"
class FormReceptionTempo;
class ClassMySql;
class ClassReceptionUdp :  public QObject
{
	Q_OBJECT
protected:
	void timerEvent(QTimerEvent *event);
public:
	ClassReceptionUdp(FormReceptionTempo * parent,ClassMySql *myClassMySql);
	void setAdresseIp(QString adresseIp);
	void setPort(uint16_t port);
	bool initSocket(void);
	void finTraitements(void);
private slots:
	void donneesRecues(void);
	void erreurSocket(QAbstractSocket::SocketError erreur);
private:
	FormReceptionTempo *laFormReceptionTempo;
	ClassMySql *myClassMySql;
	QBasicTimer timerFinColorDemandeMessage;
	QBasicTimer timerDemandeMessage;
	uint16_t port=0;
	QString adresseIp="";
	QUdpSocket *socket;
	quint16 tailleMessage=0;
	void acquitement(QByteArray data);
};

#endif

