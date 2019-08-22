


Version initiale de TeleInfo
    version  https://github.com/BmdOnline/Teleinfo du 22/02/2017

Evolutions d'une version spécifique développée par Marc Prieur 07/2019.
Pour davantage d'information, voir INSTALL.MD et https://mesrealisations.000webhostapp.com/.

Différences majeur avec la version initiale de TeleInfo:
    -La base de donnée est spécifique à l'application PiloteTempoVmc.
    -La librairie graphique est limitée à Highchart / HighStock.
    -Je n'ai pas testé la version mobile.
    -J'ai intégré/testé que la version teleinfo mode historique.
    -J'ai ajouté la possiblité en cliquant sur la courbe d'enregistrer dans la base et d'afficher des annotations.
    -Ajout de 2 courbes spécifiques vmc et fonctionnel.


# Sommaire

* [Configuration](#configuration)
    * [Accès MySQL] (#accès-mysql)
    * [Librairie graphique] (#librairie-graphique)
* [Paramètres](#paramètres)
    * [Puissance apparente - Puissance active] (#puissance-apparente---puissance-active)
    * [Gauge instantanée] (#gauge-instantanée)
        * [Donnée à afficher] (#donnée-à-afficher)
        * [Rafraichissement automatique] (#rafraichissement-automatique)
        * [Aspect des gauges] (#aspect-des-gauges)
    * [Aperçu quotidien] (#aperçu-quotidien)
    * [Historiques] (#historiques)
        * [Affichage 3D] (#affichage-3d)
        * [Type de graphique] (#type-de-graphique)
        * [Période précédente] (#période-précédente)
        * highchart version clair ou foncée
    * [Couleur des graphiques] (#couleur-des-graphiques)
* [Tarifs EDF] (#tarifs-edf)


# Configuration
## Accès MySQL
Pour commencer, il est nécessaire de définir l'accès à la base MySQL et à la table Téléinfo.
1-Renommer le fichier modele config_perso.php en config_perso.php
Dans le fichier `config_perso.php`, il faut saisir vos paramètres

Après avoir crée un utilisateur, entrez son id dans les 2 fichiers config_perso.php et  id_pilotevmc.sql 
id_pilotevmc.sql contient également le nom de la table qui doit être identique à celle entrée dans config_perso.php.
Pour générer la base sql,importer le fichier id_pilotevmc.sql contient .

A partir de là, le programme est opérationnel.

Pour le remplissage de la base, voir le programme mettre à jour..

## Librairie graphique

| HighCharts |  (http://www.highcharts.com) | propriétaire | oui | Licence propriétaire |

Afin de consommer moins de ressources, il est possible de supprimer/commenter les chargements de librairies non utilisées.

# Paramètres

## Puissance apparente - Puissance active
Dans le cas de faible consommation (<~180w), la puissance apparente (PAPP) de certains relevés téléinfo ne serait pas pertinente.
Lors de relevés téléinfo avec une fréquence réduite, la pertinence de la puissance apparente peut se poser.

Il est alors possible de recalculer la puissance active, en se basant sur l'index relevé.
Ce résultat, bien qu'approximatif (impact du cos phi), peut s'avérer préférable à la puissance apparente.

L'option se situe dans le fichier `config.php` :

/*******************************/
/*    Données EDF & Téléinfo   */
/*******************************/
//...//
$config["recalculPuissance"]     = false; // true : calcule la puissance en se basant sur le relevé d'index plutôt que PAPP
```

## Gauge instantanée
### Donnée à afficher
Il est possible d'afficher une ou deux gauges.
Dans le cas d'une seule gauge affichée, c'est la puissance qui est sélectionnée.
Dans le cas de deux gauges, l'intensité s'affichera à côté.

L'option se situe dans le fichier `config.php` :
```php
$config["graphiques"]["instantly"] = array(
    //...//
    "doubleGauge"  => true,      // true : affiche intensité en plus de la puissance
    //...//
```

### Relevé de l'index du compteur
Il est possible d'afficher l'index du compteur, pour faciliter le relevé EDF.

L'option se situe dans le fichier `config.php` :
```php
    $config["afficheIndex"]          = true;  // true : affiche les index pour chaque période tarifaire (relevé de compteur EDF)
```

### Rafraichissement automatique
Il est possible d'activer ou désactiver le rafraichissement automatique des gauges.

L'option se situe dans le fichier `config.php` :
```php
$config["graphiques"]["instantly"] = array(
    //...//
    "refreshAuto"  => true,      // active le rafraichissement automatique
    "refreshDelay" => 120,       // relancé toutes les 120 secondes
    //...//
```

### Aspect des gauges
Il est possible de modifier les différents seuils des gauges, ainsi que les couleurs associées.

Les options se situent dans le fichier `config.php` :
```php
// couleurs des bandes des gauges
$config["graphiques"]["instantly"] = array(
    //...//
    "bands" => array(            // couleurs des bandes des gauges
        "W" => array(            // Puissance
            300   => "#55BF3B",  // de 0 à 300
            1000  => "#DDDF0D",  // de 300 à 1000
            3000  => "#FFA500",  // de 1000 à 3000
            10000 => "#DF5353"   // supérieur à 3000
        ),
        "I" => array(            // Intensité
            2   => "#55BF3B",    // de 0 à 2
            5   => "#DDDF0D",    // de 2 à 5
            13  => "#FFA500",    // de 5 à 13
            100 => "#DF5353"     // supérieur à 20
        )
    )
    //...//
```

## Aperçu quotidien
Ce graphique ne propose aucun réglage spécifique.

## Historiques
### Affichage 3D
Il est possible de choisir un affichage 2D ou 3D des histogrammes (uniquemement avec HighCharts).
Cette option semble avoir quelques défauts d'affichage.

Défaut constatés :
* Les valeurs affichées sur les barres de l'histogramme sont parfois mal positionnées.
* La courbe de période précédente s'affiche derrière l'histogramme au lieu de s'afficher devant.

L'option se situe dans le fichier `config.php` :
```php
$config["graphiques"]["history"] = array(
    "show3D"     => true,       // true : affiche le graphique en 3D
    //...//
```

### Type de graphique
Il est possible de choisir le type de représentation des séries de données.
Certaines combinaisons n'ont pas de sens ou sont mal gérées par HighCharts.
A vous de tester...

Les options se situent dans le fichier `config.php` :
```php
$config["graphiques"]["history"] = array(
    //...//
    "typeSerie"  => "column",    // Type de graphique pour les séries de données (syntaxe HighCharts)
    "typePrec"   => "spline",    // Type de graphique pour les périodes précédentes (syntaxe HighCharts)
    //...//
```

Cette option n'est pas encore implémentée.

## Couleur des graphiques
Chaque donnée affiché en graphique a une couleur paramétrable.

Pour changer les couleurs, il faut adapter le fichier `config.php` :
```php
// couleurs de chacune des séries des graphiques
$teleinfo["COULEURS"] = array(
    "MIN"  => "green",   // Seuil de consommation minimale sur la période
    "MAX"  => "red",     // Seuil de consommation maximale sur la période
    "PREC" => "#DB843D", // Période précédente
    "BASE" => "#2f7ed8",
    "HP"   => "#c42525",
    "HC"   => "#2f7ed8",
    "HPJB" => "#2f7ed8",
    "HPJW" => "#8bbc21",
    "HPJR" => "#910000",
    "HCJB" => "#77a1e5",
    "HCJW" => "#a6c96a",
    "HCJR" => "#c42525",
    "HN"   => "#2f7ed8",
    "HPM"  => "#c42525",
    "I"    => "blue"     // Intensité
);


## highchart version clair ou foncée
    dans le fichiers index, commenter une de ces 2 lignes:
        <link rel="stylesheet" href="./vendor/css/jquery/ui-darkness/jquery-ui-1.12.1.min.css">   <!--fonçé  -->  
        <link rel="stylesheet" href="./vendor/css/jquery/ui-lightness/jquery-ui-1.12.1.min.css">  <!--clair  -->  

# Tarifs EDF
Le fichier `tarifs.php` contient l'historique de tous les tarifs EDF pour chaque formule.
    Les données sont nationales et communes pour tout le monde, sauf certaines taxes locales.
    * Pour un calcul plus juste, il est nécessaire d'adapter la `TCFE`.
    * A chaque évolution tarifaire, il est nécessaire d'ajouter les nouveaux tarifs dans le fichier.
    * Les arrêtés publiés au journal officiel se trouvent ici : [Facturation EDF](../ressources/Facturation EDF).
