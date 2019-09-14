// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:ClasssMySql.cpp
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-08-11 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "ClassMySql.h"
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
	SQLString ptec;
	prep_stmt->setUInt(3, compteur[0]);
	prep_stmt->setUInt(5, compteur[1]);
	prep_stmt->setUInt(7, compteur[2]);
	prep_stmt->setUInt(4, compteur[3]);
	prep_stmt->setUInt(6, compteur[4]);
	prep_stmt->setUInt(8, compteur[5]);
	int typeJour = leMessageVersBase->etatTempo & 7;
	if (typeJour != ((leMessageVersBase->etatTempo >>3) & 7))   //sinon erreur sur puissance
		ptec.append("XXXX");
	else
		ptec.append(etatPtec[typeJour]);
	prep_stmt->setBoolean(20, !typeHeure[typeJour]);
	prep_stmt->setBoolean(21, typeHeure[typeJour]);
	switch (typeJour)
	{
	case 0:
		prep_stmt->setUInt(3, leMessageVersBase->compteur);
		break;
	case 1:
		prep_stmt->setUInt(5, leMessageVersBase->compteur);
		break;
	case 2:
		prep_stmt->setUInt(7, leMessageVersBase->compteur);
		break;
	case 3:
		prep_stmt->setUInt(4, leMessageVersBase->compteur);
		break;
	case 4:
		prep_stmt->setUInt(6, leMessageVersBase->compteur);
		break;
	case 5:
		prep_stmt->setUInt(8, leMessageVersBase->compteur);
		break;
	}
	compteur[(leMessageVersBase->etatTempo >> 3) & 0xf] = leMessageVersBase->compteur; //mise a jour pour les cgt de compteur
	prep_stmt->setUInt(1, leMessageVersBase->timestamp);
	prep_stmt->setString(2, ptec);
	prep_stmt->setUInt(10, leMessageVersBase->dureeMax);
	prep_stmt->setInt(12, leMessageVersBase->tempExt);
	prep_stmt->setInt(13, leMessageVersBase->tempCuis);
	prep_stmt->setInt(14, leMessageVersBase->tempSdb);
	prep_stmt->setInt(15, leMessageVersBase->humCuis);
	prep_stmt->setInt(16, leMessageVersBase->humSdb);
	prep_stmt->setUInt(9, leMessageVersBase->iInstMax);
	prep_stmt->setInt(11, leMessageVersBase->etatWifi);
	prep_stmt->setInt(17, (leMessageVersBase->etatVmc >> 2) & 7);
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
bool  ClassMySql::traitementMessage(QByteArray  message, quint32 *delta)
{
	static quint32 memoHeure = 0;
	TeleInfoVmc* leMessageVersBase = reinterpret_cast<TeleInfoVmc*>(message.data());
	QDateTime localTime = QDateTime::fromTime_t(leMessageVersBase->timestamp, Qt::LocalTime);    //QTime::currentTime().toString()
	laFormReceptionTempo->plainTextEditMessages->appendPlainText(localTime.toString() + "__" + QString::number(leMessageVersBase->timestamp) + "__" + QString::number(leMessageVersBase->compteur) + "__" + QString::number(leMessageVersBase->iInstMax) + "__" + QString::number(leMessageVersBase->dureeMax) + "__" + QString::number(leMessageVersBase->etatTempo) + "__" + QString::number(leMessageVersBase->etatWifi) + "__" + QString::number(leMessageVersBase->tempExt) + "__" + QString::number(leMessageVersBase->tempCuis) + "__" + QString::number(leMessageVersBase->tempSdb) + "__" + QString::number(leMessageVersBase->humCuis) + "__" + QString::number(leMessageVersBase->humSdb) + "__" + QString::number(leMessageVersBase->etatVmc));
	if (memoHeure > 0)
		*delta = (leMessageVersBase->timestamp - memoHeure) / 2;
	memoHeure = leMessageVersBase->timestamp;
	if (laFormReceptionTempo->checkBoxEnregistrement->isChecked())
	{
		if (!messageVersBase(leMessageVersBase))
			return false;
	}
	return true;
}
bool ClassMySql::connexionIsOK() const
{
	return connexionOK;
}
ClassMySql::~ClassMySql()
{
	sqlStop();
}
