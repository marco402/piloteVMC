

# Projet messagesPiloteVmcVersSql partie du projet PiloteVmc(// Développé par Marc Prieur (http://mesrealisations.000webhostapp.com/))

-Transfert des messages du systeme piloteVmc vers une base de donnée mysql.
-Demande de message en Udp.
-Reception des messages en Udp.
-Affichage des messages recu.
-Transfert des messages vers la base sql.
-Demande du message suivant ou du dernier si problème de réception ou d'enregistrement.
-Possibilité de recevoir sans enregistrer.
-Surveillance cyclique de la connexion à la base de donnée.
-Demande cyclique de messages de 2 à 60 secondes en fonction de la période d'enregistrement.

# Environnement de développement

-Windows.
-Programme développé en C++ et QT.
-Ide QT créator ou visual studio.

#Installation

-Installation de QT.

-Si visual studio:
	-installation de visual studio.
	-installation de qt-vsaddin-msvc2017-2.3.2.vsix.

-Pour mysql:
	-telecharger et dezipper mysql-connector-c++-8.0.17-winx64-debug.zip--->mysqlcppconn.lib et mysqlcppconn-7-vs14.dll--->C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\
	-il faut le dossier sqldrivers dans le dossier plugins de QT:avec:mysqlcppconn.lib et mysqlcppconn-7-vs14.dll pris dans le dossier de mysql-connector-c++-8.0.17-winx64
	-telecharger mysql-connector-c++-8.0.17-src.tar.gz-->le dézipper-->copier le dossier mysql-connector-c++-8.0.17-src\jdbc\cppconn dans le dossier include de qt
	-installation de wamp64...création de la base....
*****************************************************IDE  QT creator 4.9.2**************************************************************
	-ajouter LIBS += -l"C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\mysqlcppconn" dans le .pro

***************************************************IDE Visual studio 15.9.14**************************************************************
	-projet->propriétées->editeur de lien->entrée->dépendance supplémentaire il faut:
		C:\Qt\5.13.0\msvc2017_64\plugins\sqldrivers\mysqlcppconn.lib
		C : \QT\5.13.0\MSVC2017_64\lib\Qt5Widgetsd.lib
		C : \QT\5.13.0\MSVC2017_64\lib\Qt5Guid.lib
		C : \QT\5.13.0\MSVC2017_64\lib\Qt5Networkd.lib
		C : \QT\5.13.0\MSVC2017_64\lib\Qt5Cored.lib
		C : \QT\5.13.0\MSVC2017_64\lib\qtmaind.lib
			shell32.lib
			Qt5Sqld.lib
	-Qt VS Tools->Qt Project Setting->Qt modules sélectionner SQL
	-cliquer sur le fichier .ui pour ouvrir QT designer--->dessiner l'ihm--->sauvegarder
		ensuite:console->CD chemin sources>
		C:\Qt\5.13.0\msvc2017_64\bin\uic FormReceptionTempo.ui -o ui_FormReceptionTempo.h
************************************************livraison**************************************************************************
	-console->livraison:
		-créer un dossier livraison
		-copier FormReceptionTempo.exe dans livraison
			-CD ....\livraison>
	-C:\Qt\5.13.0\msvc2017_64\bin\windeployqt.exe FormReceptionTempo.exe
		si erreur au lancement de FormReceptionTempo.exe du dossier livraison:windeployqt.exe ne transfert pas tous les modules (libay32.dll...)   verifier les modules chargés
		lancer le programme dans l'environnement, afficher les modules Déboguer-->fenêtres->modules->cliquer sur la colonne Code utilisateur transférer tous les modules .à OUI manquants dans livraison.
		ajouter les modules manquant dans la livraison.
***********************************************************************************************************************************

