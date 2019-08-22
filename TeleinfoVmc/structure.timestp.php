<?php
/************************/
/*    Table TéléInfo    */
/************************/

// Quelques informations sur Teleinfo et les formules EDF :
//   http://norm.edf.fr/pdf/HN44S812emeeditionMars2007.pdf
//   http://www.yadnet.com/index.php?page=protocole-teleinfo

// Ces données permettent au programme de fonctionner avec différentes structures de données
$config_table = array (
    // Quelques informations sur la configuration
    "type_date" => "timestamp", // "date" ou "timestamp" selon le type de stockage de la date
    // Nom des champs de la table.
    //   Clé    = nom interne au programme : NE PAS MODIFIER
    //   Valeur = nom du champ dans la table téléinfo
    // Adapter les valeurs du tableau si le nom du champ est différent
    "table" => array (
        "DATE"     => "DATE",      // => généralement, vaut soit "DATE", soit "TIMESTAMP"
        "OPTARIF"  => "OPTARIF",   // option tarifaire souscrite
        "ISOUSC"   => "ISOUSC",    // intensité souscrite
        "BASE"     => "BASE",      // BASE
        "HP"       => "HCHP",      // HCHP
        "HC"       => "HCHC",      // HCHC
        "HPJB"     => "BBRHPJB",   // BBRHPJB
        "HPJW"     => "BBRHPJW",   // BBRHPJW
        "HPJR"     => "BBRHPJR",   // BBRHPJR
        "HCJB"     => "BBRHCJB",   // BBRHCJB
        "HCJW"     => "BBRHCJW",   // BBRHCJW
        "HCJR"     => "BBRHCJR",   // BBRHCJR
        "HN"       => "EJPHN",     // EJPN
        "HPM"      => "EJPHPM",    // BBRHCJWEJPHPM
        "PTEC"     => "PTEC",      // période tarifaire en cours
        "DEMAIN"   => "DEMAIN",    // prévision du lendemain (formule Tempo)
        "IINST1"   => "IINST1",    // => généralement, vaut soit "IINST1" soit "INST1"
        "PAPP"     => "PAPP",      // puissance apparente
        "ANNOTATIONS" => "annotations"
    ),
        //marc
   "tablefonctionnel" => array (
        "ETATWIFI" => "etatWifi", // etat wifi
        "DUREEMAXI" => "dureeMax" //duree max d'un cycle dans le cycle vmc
   ),    //fin marc
        
    "tablevmc" => array (
		"TEMPEXT" => "tempExt", // température exterieur
        "TEMPCUIS" => "tempCuis", // température cuisine
        "TEMPSDB" => "tempSdb", // température salle de bain
        "HUMCUIS" => "humCuis", // humidite cuisine
        "HUMSDB" => "humSdb", // humidite salle de bain
        "MODE" => "mode", // mode vmc
        "MARCHE" => "marche", // etat vmc
        "VITESSE" => "vitesse", // vitesse vmc
 	    )    //fin marc
);
// Ces données permettent au programme de fonctionner avec différentes structures de données
//$config_table_vmc = array (
    // Quelques informations sur la configuration
//    "type_date" => "timestamp", // "date" ou "timestamp" selon le type de stockage de la date
    // Nom des champs de la table.
    //   Clé    = nom interne au programme : NE PAS MODIFIER
    //   Valeur = nom du champ dans la table téléinfo
    // Adapter les valeurs du tableau si le nom du champ est différent
//    "table" => array (
//        "DATE"     => "DATE",      // => généralement, vaut soit "DATE", soit "TIMESTAMP"

 //   )
//);
/**************************/
/*    Données TéléInfo    */
/**************************/

// Liste des valeurs possibles pour le champ "OPTARIF"
//   Clé    = valeur OPTARIF reçue par le signal Teleinfo
//   Valeur = nom interne au programme : NE PAS MODIFIER
// Adapter les clés du tableau si le contenu du champ est différent
$teleinfo["OPTARIF"] = array(
    "BASE" => "BASE",
    "HC.." => "HC",
    "BBR"  => "BBR",
    "EJP." => "EPJ"
);

// Liste des valeurs possibles pour le champ "PTEC"
//   Clé    = valeur PTEC reçue par le signal Teleinfo
//   Valeur = nom interne au programme : NE PAS MODIFIER
// Adapter les clés du tableau si le contenu du champ est différent
$teleinfo["PTEC"] = array(
    "TH.." => "BASE",
    "HP"   => "HP",
    "HC"   => "HC",
    "HPJB" => "HPJB",
    "HPJW" => "HPJW",
    "HPJR" => "HPJR",
    "HCJB" => "HCJB",
    "HCJW" => "HCJW",
    "HCJR" => "HCJR",
    "HN.." => "HN",
    "PM.." => "HPM"
);

// Liste des periodes, pour chaque option tarifaire
$teleinfo["PERIODES"] = array(
    "BASE" => array ("BASE"),
    "HC"   => array ("HP", "HC"),
    "BBR"  => array ("HPJB", "HPJW", "HPJR", "HCJB", "HCJW", "HCJR"),
    "EJP"  => array ("HN", "HPM"),
    "FONCTIONNEL"  => array ("DUREEMAXI","ETATWIFI","DUREECYCLESVMC"),   //marc séries de la courbe affichée dans l'onglet fonctionnel
    "VMC"  => array ("HUMCUIS","HUMSDB","TEMPEXT" ,"TEMPCUIS","TEMPSDB","MODE","MARCHE","VITESSE")  //marc séries de la courbe affichée dans l'onglet vmc
);

?>