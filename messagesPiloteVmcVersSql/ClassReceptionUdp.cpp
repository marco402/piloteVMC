// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistr�s vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:ClassReceptionUdp.cpp
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-07-28 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "ClassReceptionUdp.h"
ClassReceptionUdp::ClassReceptionUdp(FormReceptionTempo* parent, ClassMySql* myClassMySql) : QObject(parent) //,  QObject(myClassMySql)  //() : QObject(myClassMySql)
{
	this->myClassMySql = myClassMySql;
	laFormReceptionTempo = parent;
	socket = new QUdpSocket(this);
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));
}
bool ClassReceptionUdp::initSocket(void)
{
	if (!socket->bind(QHostAddress::Null, port))
	{
		laFormReceptionTempo->plainTextEditMessages->appendPlainText(QString::fromLatin1("Impossible de cr�er le socket en �coute"));
		return false;
	}
	connect(socket, &QUdpSocket::readyRead, this, &ClassReceptionUdp::donneesRecues);
	if (socket->isValid())
		laFormReceptionTempo->plainTextEditMessages->appendPlainText(QString::fromLatin1("Connect�."));
	else
		return false;
	timerDemandeMessage.start(DUREE_TIMER_ACQUITEMENT_MS, this);
	return true;
}
void ClassReceptionUdp::donneesRecues()
{
	QByteArray buffer;
	if (socket->hasPendingDatagrams())
	{
		buffer.resize(socket->pendingDatagramSize());
		socket->readDatagram(buffer.data(), buffer.size());
		QByteArray data;
		quint32 delta = 2;
		if (myClassMySql->traitementMessageS(buffer, &delta))
		{
			if (delta > 60)
				delta = 60;
			if (delta < 2)
				delta = 2;
			laFormReceptionTempo->labelPeriodeDemandeMessage->setText((QString::fromLatin1("P�riode timer demande de message: ") + QString::number(delta)) + "s.");
			timerDemandeMessage.start(delta * 1000, this);		//rearme le timer
			data.append("1");
			laFormReceptionTempo->plainTextEditMessages->appendPlainText("demande le paquet suivant");
		}
		else
		{
			data.append("0");
			laFormReceptionTempo->plainTextEditMessages->appendPlainText((QString::fromLatin1("redemande le m�me paquet")));
		}
		acquitement(data);
	}
}
void ClassReceptionUdp::erreurSocket(QAbstractSocket::SocketError erreur)
{
	switch (erreur) // On affiche un message diff�rent selon l'erreur qu'on nous indique
	{
	case QAbstractSocket::HostNotFoundError:
		laFormReceptionTempo->plainTextEditMessages->appendPlainText(QString::fromLatin1("ERREUR : le serveur n'a pas pu �tre trouv�. V�rifiez l'IP et le port."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		laFormReceptionTempo->plainTextEditMessages->appendPlainText(QString::fromLatin1("ERREUR : le serveur a refus� la connexion. V�rifiez si le programme \"serveur\" a bien �t� lanc�. V�rifiez aussi l'IP et le port."));
		break;
	case QAbstractSocket::RemoteHostClosedError:
		laFormReceptionTempo->plainTextEditMessages->appendPlainText(QString::fromLatin1("ERREUR : le serveur a coup� la connexion."));
		break;
	default:
		laFormReceptionTempo->plainTextEditMessages->appendPlainText("ERREUR : " + socket->errorString());
	}
}
void ClassReceptionUdp::acquitement(QByteArray data)
{
	socket->writeDatagram(data, QHostAddress(adresseIp), port);
}
void  ClassReceptionUdp::setAdresseIp(QString adresseIp)
{
	this->adresseIp = adresseIp;
}
void  ClassReceptionUdp::setPort(uint16_t port)
{
	this->port = port;
}
void ClassReceptionUdp::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == timerFinColorDemandeMessage.timerId()) {
		laFormReceptionTempo->labelLiaisonReseau->setStyleSheet(QString::fromUtf8("QLabel#labelLiaisonReseau {color:white;background-color:gray;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		timerFinColorDemandeMessage.stop();
	}
	else if (event->timerId() == timerDemandeMessage.timerId())
	{
		QByteArray data("0");
		laFormReceptionTempo->plainTextEditMessages->appendPlainText((QString::fromLatin1("redemande le m�me paquet")));
		acquitement(data); //0 renvoi du m�me,1 envoi du suivant
		laFormReceptionTempo->labelLiaisonReseau->setStyleSheet(QString::fromUtf8("QLabel#labelLiaisonReseau {color:white;background-color:green;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		timerFinColorDemandeMessage.start(1000, this);
	}
}
void ClassReceptionUdp::finTraitements()
{
	timerDemandeMessage.stop();
	socket->close();
}
