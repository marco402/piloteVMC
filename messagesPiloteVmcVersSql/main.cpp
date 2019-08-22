// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie reception des messages enregistrés vers base sql
// **************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// File:main.cpp
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
//History : V1.00 2019-07-28 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

//installation de QT.

//si IDE visual studio:
	//installation de visual studio.
	//installation de qt-vsaddin-msvc2017-2.3.2.vsix

	
//pour mysql:
	//telecharger et dezipper mysql-connector-c++-8.0.17-winx64-debug.zip--->mysqlcppconn.lib et mysqlcppconn-7-vs14.dll--->C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\
	//il faut le dossier sqldrivers dans le dossier plugins de QT:avec:mysqlcppconn.lib et mysqlcppconn-7-vs14.dll pris dans le dossier de mysql-connector-c++-8.0.17-winx64
	//telecharger mysql-connector-c++-8.0.17-src.tar.gz-->le dézipper-->copier le dossier mysql-connector-c++-8.0.17-src\jdbc\cppconn dans le dossier include de qt
	//installation de wamp64...création de la base....

//*****************************************************IDE  QT creator 4.9.2**************************************************************
	//ajouter LIBS += -l"C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\mysqlcppconn" dans le .pro

//***************************************************IDE Visual studio 15.9.14**************************************************************
	//1-projet->propriétées->editeur de lien->entrée->dépendance supplémentaire il faut:
							/*C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\mysqlcppconn.lib
							C : \QT\5.13.0\MSVC2017_64\lib\Qt5Widgetsd.lib
							C : \QT\5.13.0\MSVC2017_64\lib\Qt5Guid.lib
							C : \QT\5.13.0\MSVC2017_64\lib\Qt5Networkd.lib
							C : \QT\5.13.0\MSVC2017_64\lib\Qt5Cored.lib
							C : \QT\5.13.0\MSVC2017_64\lib\qtmaind.lib
							shell32.lib
							Qt5Sqld.lib*/
	//2-Qt VS Tools->Qt Project Setting->Qt modules sélectionner SQL
	//3-cliquer sur le fichier .ui pour ouvrir QT designer--->dessiner l'ihm--->sauvegarder
	//						ensuite:console->CD chemin sources>
	//						C:\Qt\5.13.0\msvc2017_64\bin\uic FormReceptionTempo.ui -o ui_FormReceptionTempo.h
//************************************************livraison**************************************************************************
	//-console->livraison:
							//-créer un dossier livraison
							//-copier FormReceptionTempo.exe dans livraison
							//-CD ....\livraison>
							//-C:\Qt\5.13.0\msvc2017_64\bin\windeployqt.exe FormReceptionTempo.exe
							//si erreur au lancement de FormReceptionTempo.exe du dossier livraison:windeployqt.exe ne transfert pas tous les modules (libay32.dll...)   verifier les modules chargés
							//lancer le programme dans l'environnement, afficher les modules Déboguer-->fenêtres->modules->cliquer sur la colonne Code utilisateur transférer tous les modules .à OUI manquants dans livraison.
//***********************************************************************************************************************************
#include <QtWidgets\QApplication>
#include "FormReceptionTempo.h"
#include "ui_FormReceptionTempo.h"
int main(int argc, char* argv[])
{
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
	QApplication app(argc, argv);
	QString locale = QLocale::system().name().section('_', 0, 0);
	QTranslator translator;
	translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&translator);
	FormReceptionTempo frmReceptionTempo;
	frmReceptionTempo.show();
    return app.exec();
}

