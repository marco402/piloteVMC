// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:ClasssMySql.cpp
// Written by Marc Prieur (https://marco40github.wixsite.com/website/pilotevmc)(marco40_github@sfr.fr)
//
//History : V1.00 2019-08-11 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "ClassMySql.h"
#include <QDateTime>
ClassMySql::ClassMySql(FormReceptionTempo* parent) : QObject(parent)
{
	laFormReceptionTempo = parent;
	timerTestDatabase.start(3000, this);
}
//bool ClassMySql::testConnexion(void)
//{
//	if (connection->isValid())
//	{
//		Statement *stmt;
//		try {
//			stmt = connection->createStatement();
//		}
//		catch (SQLException &ex) {
//			laFormReceptionTempo->plainTextEditMessages->appendPlainText("erreur sql getCptDernierEnregistrement : " + QString(ex.what()));
//			//textEditMessages->setText(textEditMessages->text() + "Exception occurred ErrorCode:" + QString::number(ex.getErrorCode()) + "\n");
//			//textEditMessages->setText(textEditMessages->text() + "Exception occurred SQLState:" +  ex.getSQLStateCStr() + "\n");
//			delete stmt;
//			return false;
//		}
//		return true;
//	}
//	return false;
//}
bool ClassMySql::getCptDernierEnregistrement()
{
	if (connection->isValid())
	{
		Statement *stmt;
		ResultSet *res;
		try {
			stmt = connection->createStatement();
			SQLString table = getSqlString(laFormReceptionTempo->lineEditTable->text());
			SQLString query = "Select BBRHCJB,BBRHCJW,BBRHCJR,BBRHPJB,BBRHPJW,BBRHPJR from `";
			query.append(table);
			query.append("` where DATE = (SELECT max(DATE) FROM `");
			query.append(table);
			query.append("`)");
			res = stmt->executeQuery(query);
			while (res->next()) {
				compteur[0] = res->getInt("BBRHCJB");
				compteur[1] = res->getInt("BBRHCJW");
				compteur[2] = res->getInt("BBRHCJR");
				compteur[3] = res->getInt("BBRHPJB");
				compteur[4] = res->getInt("BBRHPJW");
				compteur[5] = res->getInt("BBRHPJR");
				//////	QMessageBox::information(this, "retour", res->getString("BBRHCJB").c_str());
			}
			//////////////ok pour test
			////////////res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
			////////////while (res->next()) {
			////////////	QMessageBox::information(this, "MySQL replies:", res->getString("_message").c_str());
			////////////	QMessageBox::information(this, "MySQL says it again:", res->getString(1).c_str());
			////////////}
			//////////////fin ok

		}
		catch (SQLException &ex) {
			laFormReceptionTempo->plainTextEditMessages->appendPlainText("erreur sql getCptDernierEnregistrement : " + QString(ex.what()));
			//textEditMessages->setText(textEditMessages->text() + "Exception occurred ErrorCode:" + QString::number(ex.getErrorCode()) + "\n");
			//textEditMessages->setText(textEditMessages->text() + "Exception occurred SQLState:" +  ex.getSQLStateCStr() + "\n");
			delete res;
			delete stmt;
			return false;
		}
		delete res;
		delete stmt;
		return true;
	}
	return false;
}
bool  ClassMySql::messageVersBase(TeleInfoVmc *leMessageVersBase)
{
	SQLString etatPtec[] = { "HCJB","HCJW","HCJR","HPJB","HPJW","HPJR" };
	bool typeHeure[] = { 1,1,1,0,0,0 };
	int ordreCompteurSQL[] { 3,5,7,4,6,8 };
	SQLString ptec;
	for(int cpt=0;cpt<6;cpt++)
		prep_stmt->setUInt(ordreCompteurSQL[cpt], compteur[cpt]);
	//etattempo: 2 cas
	//soit  3 bits lsb ptec  3 bits suivant ptec: cas normal
	//soit  3 bits lsb ptec  3 bits ptec 3 bits suivant N° compteur:cas forcage des 6 compteurs
	int typeJour = leMessageVersBase->etatTempo & 7;
	int compteurCourant = (leMessageVersBase->etatTempo >> 3) & 7;
	if (typeJour != compteurCourant)   //cas forcage des 6 compteurs                 sinon erreur sur puissance
		ptec.append("XXXX");
	else                        //cas normal
		ptec.append(etatPtec[compteurCourant]);
	prep_stmt->setBoolean(20, !typeHeure[compteurCourant]);
	prep_stmt->setBoolean(21, typeHeure[compteurCourant]);
	prep_stmt->setUInt(ordreCompteurSQL[compteurCourant], leMessageVersBase->compteur);
	compteur[(leMessageVersBase->etatTempo >> 3) & 0xf] = leMessageVersBase->compteur; //mise a jour pour les cgt de compteur
	prep_stmt->setUInt(1, leMessageVersBase->timestamp);
	prep_stmt->setString(2, ptec);
	prep_stmt->setUInt(10, leMessageVersBase->dureeMax);
	prep_stmt->setInt(12, leMessageVersBase->tempExt);
	/*if(leMessageVersBase->tempCuis>255)                  pourquoi ?
		prep_stmt->setInt(13, 0);
	else*/
		prep_stmt->setInt(13, leMessageVersBase->tempCuis);
	prep_stmt->setInt(14, leMessageVersBase->tempSdb);
	prep_stmt->setInt(15, leMessageVersBase->humCuis);
	prep_stmt->setInt(16, leMessageVersBase->humSdb);
	prep_stmt->setUInt(9, leMessageVersBase->iInstMax);
	prep_stmt->setInt(11, leMessageVersBase->etatWifi);
	prep_stmt->setInt(17, (leMessageVersBase->etatVmc >> 2) & 0xf);
	prep_stmt->setInt(18, (leMessageVersBase->etatVmc >> 1) & 1);
	prep_stmt->setInt(19, leMessageVersBase->etatVmc & 1);
	try
	{
		prep_stmt->execute();
	}
	catch (SQLException &ex)
	{
		if (ex.getErrorCode() == 1062)	//date dupliquée, demande du message suivant
			return true;
		else
		{
			laFormReceptionTempo->plainTextEditMessages->appendPlainText("erreur sql traitementMessage : " + QString(ex.what()));
			//textEditMessages->setText(textEditMessages->text() + "Exception occurred ErrorCode:" + QString::number(ex.getErrorCode()) + "\n");
			//textEditMessages->setText(textEditMessages->text() + "Exception occurred SQLState:" +  ex.getSQLStateCStr() + "\n");
			QApplication::beep();
			return false;
		}
	}
	timerTestDatabase.start(5000, this);  //base ok rearme le timer
	return true;
}
bool  ClassMySql::initialisationSql()
{
	sqlStop();
	SQLString host = getSqlString(laFormReceptionTempo->lineEditDatasource->text());
	SQLString user = getSqlString(laFormReceptionTempo->lineEditUtilisateur->text());
	SQLString pass = getSqlString(laFormReceptionTempo->lineEditMotDePasse->text());
	SQLString base = getSqlString(laFormReceptionTempo->lineEditDatabase->text());
	try {
		driver = get_driver_instance();
		connection = driver->connect(host, user, pass);
		connection->setSchema(base);
		connexionOK = true;
	}
	catch (SQLException &ex) {
		laFormReceptionTempo->plainTextEditMessages->appendPlainText("ERREUR : " + QString::fromLatin1(ex.what()));
		connexionOK = false;
		return false;
	}
	return true;
}
void  ClassMySql::sqlStop()
{
	if (connexionOK) {		//sinon erreur de lecture
		connection->close();
	}
	connexionOK = false;
}
SQLString ClassMySql::getSqlString(QString texte)
{
	return SQLString((std::string(texte.toStdString())).c_str());
}
void ClassMySql::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == timerFinColorDatabase.timerId()) {
		laFormReceptionTempo->labelLiaisonDatabase->setStyleSheet(QString::fromUtf8("QLabel#labelLiaisonDatabase {color:white;background-color:gray;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		timerFinColorDatabase.stop();
	}
	else if (event->timerId() == timerTestDatabase.timerId()) {
		if (connexionOK)
		{
			if (connection->isValid())
				laFormReceptionTempo->frame_Sql->setStyleSheet(QString::fromUtf8("QFrame#frame_Sql {border-style: outset;border-width: 4px;border-radius: 10px;border-color:green;}"));
			else
				laFormReceptionTempo->frame_Sql->setStyleSheet(QString::fromUtf8("QFrame#frame_Sql {border-style: outset;border-width: 4px;border-radius: 10px;border-color:orange;}"));
		}
		else
		{
			if (initialisationSql())
				laFormReceptionTempo->frame_Sql->setStyleSheet(QString::fromUtf8("QFrame#frame_Sql {border-style: outset;border-width: 4px;border-radius: 10px;border-color:green;}"));
			else
				laFormReceptionTempo->frame_Sql->setStyleSheet(QString::fromUtf8("QFrame#frame_Sql {border-style: outset;border-width: 4px;border-radius: 10px;border-color:orange;}"));
		}
		laFormReceptionTempo->labelLiaisonDatabase->setStyleSheet(QString::fromUtf8("QLabel#labelLiaisonDatabase {color:white;background-color:green;border: 2px solid blue;border-radius: 4px;padding: 2px;}"));
		timerFinColorDatabase.start(1000, this);
	}
}
bool ClassMySql::traitementMessageS(QByteArray lesMessages, quint32 *delta) {
	QByteArray Data;
	SQLString command = "INSERT INTO ";
	command.append(getSqlString(laFormReceptionTempo->lineEditTable->text()));
	command.append(" (DATE,PTEC,BBRHCJB,BBRHPJB,BBRHCJW,BBRHPJW,BBRHCJR,BBRHPJR,IINST1,dureeMax,etatWifi,tempExt,tempCuis,tempSdb,humCuis,humSdb,mode,marche,vitesse,HCHP,HCHC) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	try {
		prep_stmt = connection->prepareStatement(command);
	}
	catch (SQLException &ex) {
		laFormReceptionTempo->plainTextEditMessages->appendPlainText("ERREUR : " + QString::fromLatin1(ex.what()));
		return false;
	}
	int nbMessage = int(lesMessages.count() / 24);
	for (int m = 0; m < nbMessage; m++)
	{
		if (!traitementMessage(lesMessages.mid(m * 24, 24),delta))
		{
			delete prep_stmt;
			return false;
		}
	}
	delete prep_stmt;
	return true;
}

//**************correction du temps 1970 suite à une coupure secteur avec la wifi coupée***************
//mémoriser tous les messages à cette date
//attendre une année > 2020
//calculer la différence entre le premier et le dernier message 1970 D1970 = 33329-28823=4506 temps passé en 1970
//enregistrer tous les messages memorises premier message 1970 = TpremMes>2000-D1970-150      = 1655550012-4506-150= 1655552868      '150 = demi - période
// les autres sont egaux au premier message 1970 recalculé +message en cours recu-premier message 1970 recu
//*****************************************************************************************************

bool  ClassMySql::traitementMessage(QByteArray  message, quint32 *delta)
{
	static quint32 memoHeure = 0;
	TeleInfoVmc* leMessageVersBase = reinterpret_cast<TeleInfoVmc*>(message.data());
	QDateTime localTime = QDateTime::fromTime_t(leMessageVersBase->timestamp, Qt::LocalTime);    //QTime::currentTime().toString()
	//**************************pb date 1970*************************************
	static QList<ClassMySql::TeleInfoVmc> messages;
	if(leMessageVersBase->timestamp < 1655540012)
	{
		messages << *leMessageVersBase;
		return true;
	}
	if (messages.count()>0)
	{ 
		uint premierTempsOK = 0;
		uint deltaTemps1970 = 0;
		uint memoT0 = messages.at(0).timestamp;
		premierTempsOK = leMessageVersBase->timestamp;
		deltaTemps1970 = messages.last().timestamp - messages.at(0).timestamp;
		messages[0].timestamp = premierTempsOK - deltaTemps1970 - 150;
	//***********************************************************
        laFormReceptionTempo->plainTextEditMessages->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "__" +  localTime.toString() + "__" + QString::number(messages[0].timestamp) + "__" + QString::number(messages[0].compteur) + "__" + QString::number(messages[0].iInstMax) + "__" + QString::number(messages[0].dureeMax) + "__" + QString::number(messages[0].etatTempo) + "__" + QString::number(messages[0].etatWifi) + "__" + QString::number(messages[0].tempExt) + "__" + QString::number(messages[0].tempCuis) + "__" + QString::number(messages[0].tempSdb) + "__" + QString::number(messages[0].humCuis) + "__" + QString::number(messages[0].humSdb) + "__" + QString::number(messages[0].etatVmc));
		if (memoHeure > 0)
			*delta = (messages[0].timestamp - memoHeure) / 2;
		memoHeure = messages[0].timestamp;
		if (laFormReceptionTempo->checkBoxEnregistrement->isChecked())
		{
			if (!messageVersBase(&messages[0]))
			{
				QApplication::beep();
				return false;
			}
		}

		//***********************************************************
		qint32 j = 0;
		for (j = 1; j < messages.count(); j++)
		{
			messages[j].timestamp = messages.at(0).timestamp + (messages.at(j).timestamp - memoT0);
			//***********************************************************
            laFormReceptionTempo->plainTextEditMessages->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "__" + localTime.toString() + "__" + QString::number(messages[j].timestamp) + "__" + QString::number(messages[j].compteur) + "__" + QString::number(messages[j].iInstMax) + "__" + QString::number(messages[j].dureeMax) + "__" + QString::number(messages[j].etatTempo) + "__" + QString::number(messages[j].etatWifi) + "__" + QString::number(messages[j].tempExt) + "__" + QString::number(messages[j].tempCuis) + "__" + QString::number(messages[j].tempSdb) + "__" + QString::number(messages[j].humCuis) + "__" + QString::number(messages[j].humSdb) + "__" + QString::number(messages[j].etatVmc));
			if (memoHeure > 0)
				*delta = (messages[j].timestamp - memoHeure) / 2;
			memoHeure = messages[j].timestamp;
			if (laFormReceptionTempo->checkBoxEnregistrement->isChecked())
			{
				if (!messageVersBase(&messages[j]))
				{
					QApplication::beep();
					return false;
				}
			}

			//***********************************************************
		}	
		messages.clear();
		return true;    // utile si test
	}
	else
	{
		//***************************end pb data 1970***************************************
        laFormReceptionTempo->plainTextEditMessages->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "__" + localTime.toString() + "__" + QString::number(leMessageVersBase->timestamp) + "__" + QString::number(leMessageVersBase->compteur) + "__" + QString::number(leMessageVersBase->iInstMax) + "__" + QString::number(leMessageVersBase->dureeMax) + "__" + QString::number(leMessageVersBase->etatTempo) + "__" + QString::number(leMessageVersBase->etatWifi) + "__" + QString::number(leMessageVersBase->tempExt) + "__" + QString::number(leMessageVersBase->tempCuis) + "__" + QString::number(leMessageVersBase->tempSdb) + "__" + QString::number(leMessageVersBase->humCuis) + "__" + QString::number(leMessageVersBase->humSdb) + "__" + QString::number(leMessageVersBase->etatVmc));
		if (memoHeure > 0)
			*delta = (leMessageVersBase->timestamp - memoHeure) / 2;
		memoHeure = leMessageVersBase->timestamp;
		if (laFormReceptionTempo->checkBoxEnregistrement->isChecked())
		{
			if (!messageVersBase(leMessageVersBase))
			{
				QApplication::beep();
				return false;
			}
		}
		return true;
	}
}
bool ClassMySql::connexionIsOK() const
{
	return connexionOK;
}
ClassMySql::~ClassMySql()
{
	sqlStop();
}
