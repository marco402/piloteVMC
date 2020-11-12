// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/

// File:FormReceptionTempo.cpp
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-08-13 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "FormReceptionTempo.h"
#pragma region debut_fin
FormReceptionTempo::FormReceptionTempo()
{
    setupUi(this);
	QObject::connect(this, SIGNAL(WindowSizeChanged()), this, SLOT(WidgetSizeChanged()));
	myClassMySql = new ClassMySql(this);
	myClassThReceptionUdp = new ClassReceptionUdp(this,myClassMySql);
	plainTextEditMessages->setToolTip(QString::fromLatin1("Date _ Compteur _ I(A) _ Durée maxi _ Etat tempo _ Etat wifi _ T ext _ T cuis _ T sdb _ Hcuis _ Hsdb _ Vmc"));
	labelLiaisonDatabase->setToolTip(QString::fromLatin1("Test database à chaque passage vert"));
	labelLiaisonReseau->setToolTip(QString::fromLatin1("Demande de message à chaque passage vert"));
	LabelCommandes->setToolTip(QString::fromLatin1("Vert en attente de messages"));
	pushButtonDepart->setText(QString::fromLatin1("Départ"));
	labelPeriodeDemandeMessage->setText((QString::fromLatin1("Période timer demande de message: ") + QString::number(DUREE_TIMER_ACQUITEMENT_MS/1000))+ "s.");
	checkBoxEnregistrement->setChecked(true);
	loadSettings();
}
FormReceptionTempo::~FormReceptionTempo()
{
	if (pushButtonDepart->text() == QString::fromLatin1("Arrêt"))
		finTraitements();
	saveSettings();
 }
#pragma endregion debut_fin
#pragma region evenements_fenetre
void FormReceptionTempo::on_pushButtonDepart_clicked()
{
	if(pushButtonDepart->text() == QString::fromLatin1("Départ"))
	{
		bool ok;
		uint16_t port  = lineEditPort->text().toUShort(&ok, 10);
		if (port < 0 || port > 65535)
		{
			plainTextEditMessages->appendPlainText(QString::fromLatin1("port hors limites(1 à 65535).\n"));
			return;
		}
		QString adresseIp = lineEditAdresseip->text().trimmed();
		if(!validateIpAdress(adresseIp))
		{
			plainTextEditMessages->appendPlainText(QString::fromLatin1("adresse ip non conforme(0.0.0.0 à 255.255.255.255) "));
			return;
		}
		if (!checkBoxEnregistrement->isChecked())
		{
			int reponse = QMessageBox::question(this, "ATTENTION", QString::fromLatin1("Sans enregistrement, les messages seront définitivement perdus."), QMessageBox::Yes | QMessageBox::Abort);
			if (reponse == QMessageBox::Abort)
			{
				return;
			}
		}
		myClassThReceptionUdp->setAdresseIp(adresseIp);
		myClassThReceptionUdp->setPort(port);
		if (checkBoxEnregistrement->isChecked())
		{
			if (myClassMySql->connexionIsOK())
			{
				if (!myClassMySql->getCptDernierEnregistrement())  //vérifier base vide
					return;
			}
			else
			{
				if (myClassMySql->initialisationSql())
				{
					if (!myClassMySql->getCptDernierEnregistrement())  //vérifier base vide
						return;
				}
				else
				{
					plainTextEditMessages->appendPlainText(QString::fromLatin1("Problème de connexion à la base avec demande d'enregistrement->abandon."));
					return;
				}
			}
		}
		plainTextEditMessages->appendPlainText("Connexion en cours...");
		if (!myClassThReceptionUdp->initSocket())
			return;
		setStateWidget(false);
		LabelCommandes->setStyleSheet(QString::fromUtf8("QLabel#LabelCommandes {color:white;background-color:green;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		pushButtonDepart->setText(QString::fromLatin1("Arrêt"));
	}
	else
	{
		LabelCommandes->setStyleSheet(QString::fromUtf8("QLabel#LabelCommandes {color:white;background-color:gray;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		pushButtonDepart->setText(QString::fromLatin1("Départ"));
		finTraitements();
	}
}
void FormReceptionTempo::finTraitements()
{
	myClassThReceptionUdp->finTraitements();
	setStateWidget(true);
}
void FormReceptionTempo::setStateWidget(bool etat)
{
	lineEditMotDePasse->setEnabled(etat);
	lineEditDatabase->setEnabled(etat);
	lineEditDatasource->setEnabled(etat);
	lineEditPort->setEnabled(etat);
	lineEditTable->setEnabled(etat);
	lineEditUtilisateur->setEnabled(etat);
	lineEditAdresseip->setEnabled(etat);
	checkBoxEnregistrement->setEnabled(etat);
}
#pragma endregion evenements_fenetre
#pragma region com
bool  FormReceptionTempo::validateIpAdress(QString adresseIp)
{
	QRegularExpression ipFormat("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	QRegularExpressionMatch mre = ipFormat.match(adresseIp);
	QStringList count = mre.capturedTexts();
	if ((count.size() > 0) && (count.contains(adresseIp)))
	{
		return true;
	}
	return false;
}
#pragma endregion com
#pragma region setting
void FormReceptionTempo::loadSettings()
{
	m_sSettingsFile = "receptionTempo.ini";   // QApplication::applicationDirPath() + "/receptionTempo.ini";
	QSettings settings(m_sSettingsFile, QSettings::IniFormat);
	if (lineEditDatasource)
		lineEditDatasource->setText(settings.value("ParamSql/lineEditDatasource", "").toString());
	if (lineEditDatabase)
		lineEditDatabase->setText(settings.value("ParamSql/lineEditDatabase", "").toString());
	if (lineEditUtilisateur)
		lineEditUtilisateur->setText(settings.value("ParamSql/lineEditUtilisateur", "").toString());
	if (lineEditMotDePasse)
		lineEditMotDePasse->setText(settings.value("ParamSql/lineEditMotDePasse", "").toString());
	if (lineEditTable)
		lineEditTable->setText(settings.value("ParamSql/lineEditTable", "").toString());
	if (lineEditAdresseip)
		lineEditAdresseip->setText(settings.value("Com/lineEditAdresseip", "").toString());
	if (lineEditPort)
		lineEditPort->setText(settings.value("Com/lineEditPort", "1000").toString());
}
void FormReceptionTempo::saveSettings()
{
	QSettings settings(m_sSettingsFile, QSettings::IniFormat);
	settings.beginGroup("ParamSql");
	settings.setValue("lineEditDatasource", (lineEditDatasource) ? lineEditDatasource->text() : "");
	settings.setValue("lineEditDatabase", (lineEditDatabase) ? lineEditDatabase->text() : "");
	settings.setValue("lineEditUtilisateur", (lineEditUtilisateur) ? lineEditUtilisateur->text() : "");
	settings.setValue("lineEditMotDePasse", (lineEditMotDePasse) ? lineEditMotDePasse->text() : "");
	settings.setValue("lineEditTable", (lineEditTable) ? lineEditTable->text() : "");
	settings.endGroup();
	settings.beginGroup("Com");
	settings.setValue("lineEditAdresseip", (lineEditTable) ? lineEditAdresseip->text() : "127.0.0.1");
	settings.setValue("lineEditPort", (lineEditTable) ? lineEditPort->text() : "1000");
	settings.endGroup();
}
#pragma endregion setting

