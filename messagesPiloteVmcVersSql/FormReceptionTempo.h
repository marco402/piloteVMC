// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:FormReceptionTempo.h
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-08-13 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef HEADER_FENRECEPTIONTEMPO
#define HEADER_FENRECEPTIONTEMPO
#include <QtGui>
#include <QtNetwork>
#include <QString.h>
#include <QMessageBox>
#include "ui_FormReceptionTempo.h"
#include "ClassMySql.h"
#include "ClassReceptionUdp.h"
class ClassMySql;
class ClassReceptionUdp;
const int DUREE_TIMER_ACQUITEMENT_MS=2000;
class FormReceptionTempo : public QWidget, public Ui::FormReceptionVmcTempo
{
    Q_OBJECT
    public:
		FormReceptionTempo();
		~FormReceptionTempo();
    private slots:
		//***********evenements fenetre***********
        void on_pushButtonDepart_clicked(void);
		void finTraitements(void);
    private:
		ClassMySql *myClassMySql;
		ClassReceptionUdp *myClassThReceptionUdp;
		void setStateWidget(bool etat);
		uint16_t port;
		QString adresseIp;
		bool validateIpAdress(QString adresseIp);
		//****setting************
		void loadSettings(void);
		void saveSettings(void);
		QString m_sSettingsFile;
};
#endif
