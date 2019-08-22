<?php
setlocale(LC_ALL , "fr_FR" );
date_default_timezone_set("Europe/Paris");
error_reporting(E_ERROR); // E_WARNING

// Adapté du code de Domos, dont il ne doit plus rester grand chose !
// cf . http://vesta.homelinux.net/wiki/teleinfo_papp_jpgraph.html

include_once("config.php");
include_once("queries.php");
include_once("tarifs.php");


////function formatMyNumber($value) { 
//    return preg_replace("/(\.\d+?)0+$/", "$1", $value)*1;
//    //return preg_replace("/(\.\d+?)0+$/", "$1", sprintf("%01.2f", $value));
// }

function getOPTARIF($full = false) {
    global $teleinfo;
    global $mysqli;

    // full : Identifie le type de résultat attendu
    //   true  :  retourne un tableau avec optarif & isousc
    //   false :  retourne optarif

    $query = queries::queryOPTARIF();

    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $result->num_rows;
    if ($nbenreg > 0) {
        $row = $result->fetch_array();
        $optarif = $teleinfo["OPTARIF"][$row["OPTARIF"]];
        $isousc = $row["ISOUSC"];
    }
    else {
      $optarif = null;
      $isousc = null;
    }

    $result->free();

    if ($full) {
        return array (
            "OPTARIF" => $optarif,
            "ISOUSC" => $isousc
        );
    } else {
        return $optarif;
    }
}

// Retourne la date du dernier relevé
// Sert pour éviter les zones vides lorsque le relevé ne se fait pas / plus
// Pas utilisé actuellement car ralentit les traitements
// Le gain ne compense pas la perte de temps.
function getMaxDate() {
    $query = queries::queryMaxDate();
    global $mysqli;

    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $result->num_rows;
    if ($nbenreg > 0) {
        $row = $result->fetch_array();
        $date = $row["DATE"];
    }
    else {
      $date = null;
    }

    $result->free();

    return $date;
}

function Tomorrow($date) {
    //return $date;
    return mktime(0, 0, 0, date("m", $date), date("d", $date)+1, date("Y", $date));
}


/****************************************/
/*    Graph consommation instantanée    */
/****************************************/
function instantly () {
    global $teleinfo;
    global $config;
    global $mysqli;

    $graphConf = $config["graphiques"]["instantly"];

    $date = isset($_GET['date'])?$_GET['date']:null;
    $optarif = isset($_GET['optarif'])?$_GET['optarif']:null;

    $heurecourante = date('H');              // Heure courante.
    $timestampheure = mktime($heurecourante+1,0,0,date("m"),date("d"),date("Y"));  // Timestamp courant à heure fixe (mn et s à 0).

    // Meilleure date entre celle donnée en paramètre et celle calculée
    $date = ($date)?min($date, $timestampheure):$timestampheure;

    $periodesecondes = 24*3600;                            // 24h.
    $timestampfin = $date;
    $timestampdebut2 = $date - $periodesecondes;           // Recule de 24h.
    $timestampdebut = $timestampdebut2 - $periodesecondes; // Recule de 24h.

    $query = queries::queryInstantly();
    $resultInst=$mysqli->query($query);
    if (!$resultInst) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $resultInst->num_rows;
    if ($nbenreg > 0) {
        $rowInst = $resultInst->fetch_array();
        $optarif = $teleinfo["OPTARIF"][$rowInst["OPTARIF"]];        $optarifStr = $teleinfo["LIBELLES"]["OPTARIF"][$optarif];
        $ptec = $teleinfo["PTEC"][$rowInst["PTEC"]];
        $ptecStr = $teleinfo["LIBELLES"]["PTEC"][$ptec];
        $demain = $rowInst["DEMAIN"];
        $date_deb = $rowInst["TIMESTAMP"];

        // Recalcul de la période
        $timestampfin = $date_deb;
        $timestampdebut2 = $date_deb - $periodesecondes;       // Recule de 24h.
        $timestampdebut = $timestampdebut2 - $periodesecondes; // Recule de 24h.

        $Isousc = floatval(str_replace(",", ".", $rowInst["ISOUSC"]));
        $val["W"] = floatval(str_replace(",", ".", $rowInst["PAPP"]));
        $series["W"] = "Watts";

        $query = queries::queryMaxPeriod ($timestampdebut2, $timestampfin, $optarif);
        $resultMax=$mysqli->query($query);
        if (!$resultMax) {
            printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
            exit();
        }
        $nbenreg = $resultMax->num_rows;
        $haveI = false;
        if ($nbenreg > 0) {
            
           // if ($config["recalculPuissance"])
           // {

           // }
            //else
           // {
                $max["W"] = max($val["W"], $rowMax["PAPP"]);
                $rowMax = $resultMax->fetch_array();
           // }    
            // Intensités IINST1... IINST3
            if ($graphConf["intensity"]) {
                $numPhase = 1;
                while (isset($rowMax["IINST" . $numPhase])) {
                    $val["I" . $numPhase] = floatval(str_replace(",", ".", $rowInst["IINST" . $numPhase]));
                    $max["I" . $numPhase] = max($val["I" . $numPhase], $rowMax["IINST" . $numPhase]);
                    $haveI = $haveI || ($max["I" . $numPhase]!=0);
                    $numPhase++;
                }
            }
            // Différents index
            if ($config["afficheIndex"]) {
                foreach($teleinfo["PERIODES"][$optarif] as $field) {
                    $index[$field] = array (
                        "title" => $teleinfo["LIBELLES"]["PTEC"][$field],
                        "value" => $rowMax[$field],
                    );
                }
            }
        };
        $resultMax->free();
        $resultInst->free();

        $datetext = date("d/m G:i", $date_deb);

        $seuils["W"] = array (
            'min' => 0,
            'max' => ceil($max["W"] / 500) * 500, // Arrondi à 500 "au dessus"
        );
        $bands["W"] = $graphConf["bands"]["W"];

        // Subtitle pour la période courante
        $subtitle = "Option tarifaire : <b>".$optarifStr." (".$Isousc." A)</b><br />";
        $subtitle .= "Période tarifaire : <b>".$ptecStr."</b><br />";
        switch ($optarif) {
            case "BBR":
                $subtitle .= "Prochaine période : <b>".$demain."</b><br />";
                break;
            default :
                break;
        }
        $subtitle .= "Puissance Max : <b>".intval($max["W"])." W</b><br />";

        // Affiche l'intensité ?
        if (($graphConf["intensity"]) && $haveI) {
            // Intensités IINST1... IINST3
            $subtitle .= "Intensité Max : <b>";
            $numPhase = 1;
            while (isset($rowMax["IINST" . $numPhase])) {
                $val["I" . $numPhase] = floatval(str_replace(",", ".", $rowInst["IINST" . $numPhase]));
                $series["I" . $numPhase] = "Ampères";

                $seuils["I" . $numPhase] = array (
                    'min' => 0,
                    'max' => ceil($max["I" . $numPhase] / 5) * 5, // Arrondi à 5 "au dessus"
                );
                $bands["I" . $numPhase] = $graphConf["bands"]["I"];

                $subtitle .= intval($max["I" . $numPhase])." A / ";
                $numPhase++;
            }
            // Supprime les 3 derniers caractères : " / "
            $subtitle = substr($subtitle, 0, -3);

            $subtitle .= "</b><br />";
        }

        // Différents index
        if ($config["afficheIndex"]) {
            foreach($teleinfo["PERIODES"][$optarif] as $field) {
                $subtitle .= "Index " . $field . " : <b>" . $index[$field]["value"]/1000 . " KWh</b><br />"; // Le compteur affiche la valeur / 1000
            }
        }

        $instantly = array(
            'title' => "Consommation du $datetext",
            'subtitle' => $subtitle,
            'optarif' => array($optarif => $optarifStr),
            'index' => $index,
            'ptec' => array($ptec => $ptecStr),
            'demain' => $demain,
            'debut' => $date_deb*1000, // $date_deb_UTC,
            'series' => $series,
            'data'=> $val,
            'seuils' => $seuils,
            'bands' => $bands,
            'refresh_auto' => $graphConf["refreshAuto"],
            'refresh_delay' => $graphConf["refreshDelay"]
        );

        return $instantly;
    } else {
        $resultInst->free();
        return null;
    }
}

/******************************************************************************************/
/*    Graph consommation w des 24 dernières heures + en parrallèle consommation d'Hier    */
/******************************************************************************************/
function daily () {
    global $teleinfo;
    global $config;
    global $mysqli;

    $graphConf = $config["graphiques"]["daily"];
    //$date = isset($_GET['date'])?$_GET['date']:null;
    $date = isset($_GET['date'])?min($_GET['date'],getMaxDate()):getMaxDate();
    $heurecourante = date('H');              // Heure courante.
    $timestampheure = mktime($heurecourante+1,0,0,date("m"),date("d"),date("Y"));  // Timestamp courant à heure fixe (mn et s à 0)

    // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
    $date = ($date)?min($date, $timestampheure):$timestampheure;
    $annotation=array();
   // $points_annotation=array();
    $periodesecondes = 24*3600;                            // 24h.
    $timestampfin = $date;
    $timestampdebut = $date - $periodesecondes;           // Recule d'une période
   // $timestampdebut = $timestampdebut2 - $periodesecondes; // Recule d'une période

    if ($config["recalculPuissance"]) {
        $tab_optarif = getOPTARIF(true);
        $optarif = $tab_optarif["OPTARIF"];
    } else {
        $optarif = null;
    }

    $query = queries::queryDaily($timestampdebut, $timestampfin, $optarif);
    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }

    $pourtest=null;
    $nbenreg = $result->num_rows;
    if ($nbenreg > 0) {
        $date_deb=0; // date du 1er enregistrement
        $date_fin=time();

        $row = $result->fetch_array();
        $optarif = $teleinfo["OPTARIF"][$row["OPTARIF"]];
        //$optarifStr = $teleinfo["LIBELLES"]["OPTARIF"][$teleinfo["OPTARIF"][$optarif]];
        $optarifStr = $teleinfo["LIBELLES"]["OPTARIF"][$optarif];
        $ptec = $teleinfo["PTEC"][$row["PTEC"]];
        $ptecStr = $teleinfo["LIBELLES"]["PTEC"][$ptec];

        // Initialisation des courbes qui seront affichées
        foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
            $courbe_titre[$ptec]=$teleinfo["LIBELLES"]["PTEC"][$ptec];
            $courbe_color[$ptec]=$teleinfo["COULEURS"][$ptec];
            $courbe_min[$ptec]=5000;
            $courbe_max[$ptec]=0;
            $courbe_mindate[$ptec]=null;
            $courbe_maxdate[$ptec]=null;
            $array[$ptec]=array();
        }

        // Intensités IINST1... IINST3
        if ($graphConf["intensity"]) {
            $numPhase = 1;
            while (isset($row["IINST" . $numPhase])) {
                // Ajout des courbes intensités
                $courbe_titre["I" . $numPhase]="Intensité " . $numPhase;
                $courbe_color["I" . $numPhase]=$teleinfo["COULEURS"]["I" . $numPhase] ? $teleinfo["COULEURS"]["I" . $numPhase] : $teleinfo["COULEURS"]["I"];
                $courbe_min["I" . $numPhase]=5000;
                $courbe_max["I" . $numPhase]=0;
                $courbe_mindate["I" . $numPhase]=null;
                $courbe_maxdate["I" . $numPhase]=null;
                $array["I" . $numPhase] = array();
                $numPhase++;
            }
        }
        $A = array();
        $result->data_seek(0); // Revient au début (car on a déjà lu un enreg)
        $prevptec = null;
        $prevts = null;
        $prevval = array();
        $previdx = array();
        
        while ($row = $result->fetch_array()) {
            $ts = intval($row["TIMESTAMP"]);
            $curptec = $teleinfo["PTEC"][$row["PTEC"]];
            if ($curptec!==null) 
            { 
                  // Période courante
                if ($date_deb==0) {
                    $date_deb = $row["TIMESTAMP"];
                }
                $ts = $ts * 1000;
                if ($config["recalculPuissance"]) {
                    // On recalcule la puissance active, basée sur les relevés d'index
                    $curidx = floatval(str_replace(",", ".", $row[$curptec]));
                    $deltaidx = $curidx-$previdx[$curptec];
                     if($deltaidx>0)
                    {
                        $deltats = ($ts - $prevts) / 1000 / 60; // en minutes
                        if ($previdx[$curptec]!==null) {
                            $val =  $deltaidx / $deltats * 60;
                          }
                        $A[0]=$previdx[$curptec];
                        $previdx[$curptec] = $curidx;
                    }
                    else {
                       $val=0;
                    }
                }
                else 
                  // On utilise la puissance apparente  ($ts - $prevts)<300000
                     $val = floatval(str_replace(",", ".", $row["PAPP"]));
                  $prevval[$prevptec]= $val;
                // Affecte la consommation selon la période tarifaire
                foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
                    if ($curptec == $ptec) {
                        // Période tarifaire courante
                        $array[$ptec][] = array($ts, $val); // php recommande cette syntaxe plutôt que array_push
                    }  else {
                        // Toutes les autres périodes tarifaires
                        $array[$ptec][] = array($ts, null);
                    }
                }
                // Ajuste les seuils min/max le cas échéant
                if ($courbe_max[$curptec] < $val) {$courbe_max[$curptec] = $val; $courbe_maxdate[$curptec] = $ts;};
                if ($courbe_min[$curptec] > $val) {$courbe_min[$curptec] = $val; $courbe_mindate[$curptec] = $ts;};

                // Highstock permet un navigateur chronologique
                $navigator[] = array($ts, $val);

                //traitement des annotations y=compteur courant
                $annot=$row["ANNOTATIONS"] ;            
                if($annot !== null & $annot !== "")
                {
                    unset($points_annotation);
                    $points_annotation[] = array('x' => $ts,'y' => $val,'xAxis' => 0, 'yAxis' => 0);
                    $annotation[] = array('point'=>$points_annotation,'text'=>$annot);
                }
                // Intensités IINST1... IINST3
                if ($graphConf["intensity"]) {
                    $numPhase = 1;
                    while (isset($row["IINST" . $numPhase])) {
                        $val = floatval(str_replace(",", ".", $row["IINST" . $numPhase]));
                        $array["I" . $numPhase][] = array($ts, $val); // php recommande cette syntaxe plutôt que array_push
                        if ($courbe_max["I" . $numPhase] < $val) {$courbe_max["I" . $numPhase] = $val; $courbe_maxdate["I" . $numPhase] = $ts;};
                        if ($courbe_min["I" . $numPhase] > $val) {$courbe_min["I" . $numPhase] = $val; $courbe_mindate["I" . $numPhase] = $ts;};
                        $numPhase++;
                    }
                }
                 $prevptec = $curptec;
            }   //if ($curptec!==null) 
        $prevts = $ts;
        }   //while
        $result->free();

        $date_fin = $ts/1000;

        $plotlines_max =  max(array_diff_key($courbe_max, array("I1"=>null, "I2"=>null, "I3"=>null, "PREC"=>null)));
        $plotlines_min =  min(array_diff_key($courbe_min, array("I1"=>null, "I2"=>null, "I3"=>null, "PREC"=>null)));




        $date_deb_UTC=$date_deb*1000;
        $datetext = date("d/m G:i", $date_deb) . " au " . date("d/m G:i", $date_fin);

        $seuils = array (
            'min' => number_format($plotlines_min, 0, '.', ''),
            'max' => number_format($plotlines_max, 0, '.', ''),
        );

        // Conserve les séries nécessaires
        $series = array_intersect_key($teleinfo["LIBELLES"]["PTEC"], array_flip($teleinfo["PERIODES"][$optarif]));

        $daily = array(
            'title' => "Tempo du $datetext",
            'subtitle' => "",
            'optarif' => array($optarif => $optarifStr),
            'ptec' => array($ptec => $ptecStr),
            'debut' => $timestampfin*1000, // $date_deb_UTC,
            'series' => $series,
            'MAX_color' => $teleinfo["COULEURS"]["MAX"],
            'MIN_color' => $teleinfo["COULEURS"]["MIN"],
            'navigator' => $navigator,
            'seuils' => $seuils,
            'annotation' => $annotation  //,
            //'points_annotation' => $points_annotation
        );

        foreach($courbe_max as $key => $value) {
            //$courbe_max [$key]=formatMyNumber($value);
            $courbe_max [$key]= number_format($value, 0, '.', '');
        }
        foreach($courbe_min as $key => $value) {
            // $courbe_min [$key]=formatMyNumber($value);
            $courbe_min [$key]= number_format($value, 0, '.', '');
        }

        // Ajoute les séries
        foreach(array_keys($array) as $ptec) {
            $daily[$ptec."_name"] = $courbe_titre[$ptec]." [".$courbe_min[$ptec]." ~ ".$courbe_max[$ptec]."]";
            $daily[$ptec."_color"] = $courbe_color[$ptec]; // $teleinfo["COULEURS"][$ptec];
            $daily[$ptec."_data"] = $array[$ptec];
        }

        return $daily;
    } else {
        $result->free();
        return null;
    }
}

/******************************************************************************************/
/*    Graph consommation w des 24 dernières heures + en parrallèle consommation d'Hier    */
/******************************************************************************************/
//SELECT * FROM `pilotevmctempo` WHERE `PTEC` != 'HCJB' && `PTEC` != 'HPJB'
//SELECT * FROM `pilotevmctempo` WHERE `BBRHCJW` != 507909
function get_fonctionnel () {     //marc
    global $teleinfo;
    global $config;
    global $mysqli;

    //$graphConf = $config["graphiques"]["vmc"];   //"detailPrec" et "titre"

    //*********************gestion du temps inchangé**************************
    //$date = isset($_GET['date'])?$_GET['date']:null;
    $date = isset($_GET['date'])?min($_GET['date'],getMaxDate()):getMaxDate();
    $annotation=array();
    $heurecourante = date('H');              // Heure courante.
    $timestampheure = mktime($heurecourante+1,0,0,date("m"),date("d"),date("Y"));  // Timestamp courant à heure fixe (mn et s à 0)

    // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
    $date = ($date)?min($date, $timestampheure):$timestampheure;

    $periodesecondes = 24*3600;                            // 24h.
    $timestampfin = $date;
    $timestampdebut = $date - $periodesecondes;    // Recule d'une période
    //******************lecture de la base mysql*******************************
    $query = queries::queryFonctionnel($timestampdebut, $timestampfin);
    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $result->num_rows;
    //****************************traitement des enregistrements************
    if ($nbenreg > 0) {
        $date_deb=0; // date du 1er enregistrement
        $date_fin=time();
        //$row = $result->fetch_array();
        $indiceY=0;
        // Initialisation des courbes qui seront affichées
        foreach($teleinfo["PERIODES"]["FONCTIONNEL"] as $etat) {
            $courbe_titre[$etat]=$teleinfo["LIBELLES"]["titres_vmc"][$etat];;
            $courbe_color[$etat]=$teleinfo["COULEURS"][$etat];          
            $courbe_min[$etat]=5000;
            $courbe_max[$etat]=0;
            $courbe_mindate[$etat]=null;
            $courbe_maxdate[$etat]=null;
            $array[$etat]=array();
            $yaxis[$etat]=$indiceY;
            $indiceY=$indiceY+1;
         }

     //************************** */parcourt des enregistrements********************************
       // $result->data_seek(0); // Revient au début (car on a déjà lu un enreg)
       // $prevptec = null;
       // $prevts = null;
       // $previdx = array();
        $ts = 0;
        $tsprec=0;
        $deltat=0;
        $passe=false;
        while ($row = $result->fetch_array()) {
            //**********temps*****************
                $ts = intval($row["TIMESTAMP"]);
                if ($date_deb==0) {
                    $date_deb = $ts;          //en seconde 1970
                     $tsprec=$ts;
                }
                $deltat=$ts-$tsprec;
                $tsprec=$ts;

                $ts = $ts * 1000;                           //en ms 1970
             
               // $deltats = ($ts - $prevts) / 1000 / 60;     // en minutes
               // $prevts = $ts; 

               //traitement duree maxi                             //en ms
                    $val = intval(str_replace(",", ".", $row["DUREEMAXI"]));
                    $array["DUREEMAXI"][] = array($ts, $val); 
                    // Ajuste les seuils min/max le cas échéant
                    if ($courbe_max["DUREEMAXI"] < $val) {$courbe_max["DUREEMAXI"] = $val; $courbe_maxdate["DUREEMAXI"] = $ts;};
                    if ($courbe_min["DUREEMAXI"] > $val) {$courbe_min["DUREEMAXI"] = $val; $courbe_mindate["DUREEMAXI"] = $ts;};

                    // Highstock permet un navigateur chronologique
                    $navigator[] = array($ts, $val);
                //traitement des annotations y=compteur courant
                $annot=$row["ANNOTATIONS"] ;            
                if($annot !== null & $annot !== "")
                {
                    unset($points_annotation);
                    $points_annotation[] = array('x' => $ts,'y' => $val,'xAxis' => 0, 'yAxis' => 0);
                    $annotation[] = array('point'=>$points_annotation,'text'=>$annot);
                }
                //traitement wifi  
                    $val = intval(str_replace(",", ".", $row["ETATWIFI"]));
                    $array["ETATWIFI"][] = array($ts, $val); 
                    if ($courbe_max["ETATWIFI"] < $val) {$courbe_max["ETATWIFI"] = $val; $courbe_maxdate["ETATWIFI"] = $ts;};
                    if ($courbe_min["ETATWIFI"] > $val) {$courbe_min["ETATWIFI"] = $val; $courbe_mindate["ETATWIFI"] = $ts;};
                //traitement data delta temps cycle vmc
                if($passe==true)
                {
                    $val = intval(str_replace(",", ".", $deltat));
                    $array["DUREECYCLESVMC"][] = array($ts, $val); 
                    if ($courbe_max["DUREECYCLESVMC"] < $val) {$courbe_max["DUREECYCLESVMC"] = $val; $courbe_maxdate["DUREECYCLESVMC"] = $ts;};
                    if ($courbe_min["DUREECYCLESVMC"] > $val) {$courbe_min["DUREECYCLESVMC"] = $val; $courbe_mindate["DUREECYCLESVMC"] = $ts;};
                }
                $passe=true;
                
          }
        //$result->free();

        $date_fin = $ts/1000;         //en seconde 1970
      
        $plotlines_max =  max(array_diff_key($courbe_max, array("I1"=>null, "I2"=>null, "I3"=>null, "PREC"=>null)));
        $plotlines_min =  min(array_diff_key($courbe_min, array("I1"=>null, "I2"=>null, "I3"=>null, "PREC"=>null)));
        $seuils = array (
                    'min' => 0,  //number_format($plotlines_min, 0, '.', ' '),
                    'max' => 3000,  //number_format( $plotlines_max, 0, '.', ' '),
                );

        $date_deb_UTC=$ts;  //en ms 1970
        $datetext = date("d/m G:i", $date_deb) . " au " . date("d/m G:i", $date_fin);
      

        // Conserve les séries nécessaires
       // $series = array_intersect_key($teleinfo["LIBELLES"]["PTEC"], array_flip($teleinfo["PERIODES"][$optarif]));
        $series["DUREEMAXI"] = "DUREEMAXI";
        $series["ETATWIFI"] = "ETATWIFI";
        $series["DUREECYCLESVMC"] = "DUREECYCLESVMC";
        $etat_daily = array(
            'title' => "Fonctionnel du $datetext",
            'subtitle' => "",
            //'data'=>  $val,
            //'optarif' => array($optarif => $optarifStr),
            //'ptec' => array($ptec => $ptecStr),
            //'$dureemax' =>  $array[$dureeMax],                 //
            'debut' =>$date_fin*1000,  //, // $date_deb_UTC,
            'series' => $series,
            'MAX_color' => $teleinfo["COULEURS"]["MAX"],
            'MIN_color' => $teleinfo["COULEURS"]["MIN"],
            'navigator' => $navigator,
            'seuils' => $seuils ,
            'annotation' => $annotation
        );
        foreach($courbe_max as $key => $value) {
            //$courbe_max [$key]=formatMyNumber($value);
            $courbe_max [$key]= number_format($value, 0, '.', '');
        }
        foreach($courbe_min as $key => $value) {
            // $courbe_min [$key]=formatMyNumber($value);
            $courbe_min [$key]= number_format($value, 0, '.', '');
        }
        // Ajoute les séries
        foreach(array_keys($array) as $fonctionnel) {
            $etat_daily[$fonctionnel."_name"] = $courbe_titre[$fonctionnel]." [".$courbe_min[$fonctionnel]." ~ ".$courbe_max[$fonctionnel]."]";
            $etat_daily[$fonctionnel."_color"] = $courbe_color[$fonctionnel]; // $teleinfo["COULEURS"][$dureeMax];
            $etat_daily[$fonctionnel."_data"] =  $array[$fonctionnel];
            $etat_daily[$fonctionnel."_yaxis"] =  $yaxis[$fonctionnel];
            $etat_daily[$fonctionnel."_ymax"] =  $courbe_max[$fonctionnel];
        }
       // $result->free();
        return $etat_daily;
    } else {
        $result->free();
        return null;
    }
}
/******************************************************************************************/
/*    Graph consommation w des 24 dernières heures + en parrallèle consommation d'Hier    */
/******************************************************************************************/
//SELECT * FROM `pilotevmctempo` WHERE `PTEC` != 'HCJB' && `PTEC` != 'HPJB'
//SELECT * FROM `pilotevmctempo` WHERE `BBRHCJW` != 507909
function get_vmc () {     //marc
    global $teleinfo;
    global $config;
    global $mysqli;

    //$graphConf = $config["graphiques"]["vmc"];   //"detailPrec" et "titre"

    //*********************gestion du temps inchangé**************************
    //$date = isset($_GET['date'])?$_GET['date']:null;
    $date = isset($_GET['date'])?min($_GET['date'],getMaxDate()):getMaxDate();
    $heurecourante = date('H');              // Heure courante.
    $timestampheure = mktime($heurecourante+1,0,0,date("m"),date("d"),date("Y"));  // Timestamp courant à heure fixe (mn et s à 0)

    // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
    $date = ($date)?min($date, $timestampheure):$timestampheure;
    $annotation=array();
    $periodesecondes = 24*3600;                            // 24h.
    $timestampfin = $date;
    $timestampdebut = $date - $periodesecondes;             // Recule d'une période
   // $timestampdebut = $timestampdebut2 - $periodesecondes; // Recule d'une période
    //******************lecture de la base mysql*******************************
    $query = queries::queryVmc($timestampdebut, $timestampfin);
    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $result->num_rows;
    //****************************traitement des enregistrements************
    if ($nbenreg > 0) {
        $date_deb=0; // date du 1er enregistrement
        $date_fin=time();
        //$row = $result->fetch_array();
        $indiceY=0;
        // Initialisation des courbes qui seront affichées
        foreach($teleinfo["PERIODES"]["VMC"] as $etat) {
            $courbe_titre[$etat]=$teleinfo["LIBELLES"]["titres_vmc"][$etat];;
            $courbe_color[$etat]=$teleinfo["COULEURS"][$etat];          
            $courbe_min[$etat]=5000;
            $courbe_max[$etat]=0;
            $courbe_mindate[$etat]=null;
            $courbe_maxdate[$etat]=null;
            $array[$etat]=array();
            if($indiceY>4)
                 $yaxis[$etat]=2;   //etat
            else if($indiceY>1)
                 $yaxis[$etat]=1;   //temperature
            else 
                 $yaxis[$etat]=0;   //humidité
            $indiceY=$indiceY+1;
         }

     //************************** */parcourt des enregistrements********************************
       // $result->data_seek(0); // Revient au début (car on a déjà lu un enreg)
       // $prevptec = null;
       // $prevts = null;
       // $previdx = array();
        $ts = 0;
        $tsprec=0;
        $deltat=0;
        //$plotlines_min=0;
        while ($row = $result->fetch_array()) {
            //**********temps*****************
                $ts = intval($row["TIMESTAMP"]);
                if ($date_deb==0) {
                    $date_deb = $ts;          //en seconde 1970
                     $tsprec=$ts;
                }
                $deltat=$ts-$tsprec;
                $tsprec=$ts;

                $ts = $ts * 1000;                           //en ms 1970
             
               // $deltats = ($ts - $prevts) / 1000 / 60;     // en minutes
               // $prevts = $ts; 

               foreach($teleinfo["PERIODES"]["VMC"] as $courbe) {
              $val = intval(str_replace(",", ".", $row[$courbe]));
              if($courbe=='TEMPSDB' | $courbe=='TEMPCUIS' |$courbe=='TEMPEXT' )
                $val = $val/10.0;
              $array[$courbe][] = array($ts, $val); 
              if ($courbe_max[$courbe] < $val) {$courbe_max[$courbe] = $val; $courbe_maxdate[$courbe] = $ts;};
              if ($courbe_min[$courbe] > $val) {$courbe_min[$courbe] = $val; $courbe_mindate[$courbe] = $ts;};
              if ($plotlines_max < $val) $plotlines_max = $val;
              if ($plotlines_min > $val) $plotlines_min = $val;

               }
               //traitement des annotations y=compteur courant
               $annot=$row["ANNOTATIONS"] ;            
               if($annot !== null & $annot !== "")
               {
                   unset($points_annotation);
                   $points_annotation[] = array('x' => $ts,'y' => $val,'xAxis' => 0, 'yAxis' => 0);
                   $annotation[] = array('point'=>$points_annotation,'text'=>$annot);
               }
 
                
          }
        //$result->free();

             $date_fin = $ts/1000;         //en seconde 1970

       // $T_max = max(array_diff_key($courbe_max, array("TEMPEXT"=>null, "TEMPCUIS"=>null, "TEMPSDB"=>null)));
       // $T_min =min(array_diff_key($courbe_min, array("TEMPEXT"=>null, "TEMPCUIS"=>null, "TEMPSDB"=>null)));
       // $H_max = max(array_diff_key($courbe_max, array("HUMCUIS"=>null, "HUMSDB"=>null)));
       // $H_min =min(array_diff_key($courbe_min, array("HUMCUIS"=>null, "HUMSDB"=>null)));
       // $E_max = max(array_diff_key($courbe_max, array("MODE"=>null, "MARCHE"=>null, "VITESSE"=>null)));
       // $E_min =min(array_diff_key($courbe_min, array("MODE"=>null, "MARCHE"=>null, "VITESSE"=>null)));
        $seuils = array (
                    'min' => $plotlines_min,
                    'max' => $plotlines_max,
                );

        $date_deb_UTC=$ts;  //en ms 1970
        $datetext = date("d/m G:i", $date_deb) . " au " . date("d/m G:i", $date_fin);
      

        // Conserve les séries nécessaires
       // $series = array_intersect_key($teleinfo["LIBELLES"]["PTEC"], array_flip($teleinfo["PERIODES"][$optarif]));

       foreach($teleinfo["PERIODES"]["VMC"] as $courbe)
            $series[$courbe] = $courbe;     
 

       // $series["DUREEMAXI"] = "DUREEMAXI";
     
        $etat_daily = array(
            'title' => "Vmc du $datetext",
            'subtitle' => "",
            //'data'=>  $val,
            //'optarif' => array($optarif => $optarifStr),
            //'ptec' => array($ptec => $ptecStr),
            //'$dureemax' =>  $array[$dureeMax],                 //
            'debut' =>$date_fin*1000,  //, // $date_deb_UTC,
            'series' => $series,
            'MAX_color' => $teleinfo["COULEURS"]["MAX"],
            'MIN_color' => $teleinfo["COULEURS"]["MIN"],
           // 'navigator' => $navigator,
            'seuils' => $seuils,
            'annotation' => $annotation
        );

        // Ajoute les séries
        foreach(array_keys($array) as $vmc) {
            $etat_daily[$vmc."_name"] = $courbe_titre[$vmc]." [".$courbe_min[$vmc]." ~ ".$courbe_max[$vmc]."]";
            $etat_daily[$vmc."_color"] = $courbe_color[$vmc]; // $teleinfo["COULEURS"][$dureeMax];
            $etat_daily[$vmc."_data"] =  $array[$vmc];
            $etat_daily[$vmc."_yaxis"] =  $yaxis[$vmc];
        }
       // $result->free();
        return $etat_daily;
    } else {
        $result->free();
        return null;
    }
}
/*************************************************************/
/*    Graph cout sur période [8jours|8semaines|8mois|1an]    */
/*************************************************************/
function history() {
    global $teleinfo;
    global $config;
    global $mysqli;

    $graphConf = $config["graphiques"]["history"];

    $duree = isset($_GET['duree'])?$_GET['duree']:8;
    $periode = isset($_GET['periode'])?$_GET['periode']:"jours";
    //$date = isset($_GET['date'])?$_GET['date']:null;
    $date = isset($_GET['date'])?min($_GET['date'],Tomorrow(getMaxDate())):Tomorrow(getMaxDate());

    switch ($periode) {
        case "jours":
            // Calcul de la fin de période courante
            $timestampheure = mktime(0,0,0,date("m"),date("d"),date("Y"));   // Timestamp courant, 0h
            $timestampheure += 24*3600;                                      // Timestamp courant +24h

            // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
            $date = ($date)?min($date, $timestampheure):$timestampheure;

            // Périodes
            $decalage = '-' . $duree . ' day';                              // Période, en texte
            $timestampfin = $date;                                           // Fin de la période
            $timestampdebut = strtotime($decalage, $timestampfin);          // Début de période active
            //$timestampdebut = strtotime($decalage, $timestampdebut2);        // Début de période précédente

            $xlabel = $duree . ($duree==1 ? " jour" : " jours");
            $dateformatsql = "%a %e";
            $divabonnement = 365;
            break;
        case "semaines":
            // Calcul de la fin de période courante
            $timestampheure = mktime(0,0,0,date("m"),date("d"),date("Y"));   // Timestamp courant, 0h
            $timestampheure += 24*3600;                                      // Timestamp courant +24h

            // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
            $date = ($date)?min($date, $timestampheure):$timestampheure;

            // Avance d'un jour tant que celui-ci n'est pas un lundi
            while ( date("w", $date) != 1 ) {
                $date += 24*3600;
            }

            // Périodes
            $decalage = '-' . $duree . ' week';                              // Période, en texte
            $timestampfin = $date;                                           // Fin de la période
            $timestampdebut = strtotime($decalage, $timestampfin);          // Début de période active
           // $timestampdebut = strtotime($decalage, $timestampdebut2);        // Début de période précédente

            $xlabel = $duree . ($duree==1 ? " semaine" : " semaines");
            $dateformatsql = "sem %v (%x)";
            $divabonnement = 52;
            break;
        case "mois":
            // Calcul de la fin de période courante
            $timestampheure = mktime(0,0,0,date("m"),date("d"),date("Y")); // Timestamp courant, 0h

            // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
            $date = ($date)?min($date, $timestampheure):$timestampheure;

            // Avance d'un jour tant qu'on n'est pas le premier du mois
            while ( date("d", $date) != 1 ) {
                $date += 24*3600;
            }

            // Périodes
            $decalage = '-' . $duree . ' month';                             // Période, en texte
            $timestampfin = $date;                                           // Fin de la période
            $timestampdebut = strtotime($decalage, $timestampfin);          // Début de période active
            //$timestampdebut = strtotime($decalage, $timestampdebut2);        // Début de période précédente

            $xlabel = $duree . ($duree==1 ? " mois" : " mois"); // Prévision pour intl
            $dateformatsql = "%b (%Y)";
            if ($duree > 6) $dateformatsql = "%b %Y";
            $divabonnement = 12;
            break;
        case "ans":
            // Calcul de la fin de période courante
            $timestampheure = mktime(0,0,0,date("m"),date("d"),date("Y"));         // Timestamp courant, 0h

            // Meilleure date entre celle donnée en paramètre, celle calculée et la dernière date en base
            $date = ($date)?min($date, $timestampheure):$timestampheure;

            $date = mktime(0,0,0,1,1,date("Y", $date)+1);                          // Année suivante, 0h

            // Périodes
            $decalage = '-' . $duree . ' year';                              // Période, en texte
            $timestampfin = $date;                                           // Fin de la période
            $timestampdebut = strtotime($decalage, $timestampfin);          // Début de période active
           // $timestampdebut = strtotime($decalage, $timestampdebut2);        // Début de période précédente

            $xlabel = $duree . ($duree==1 ? " an" : " ans");
            $dateformatsql = "%b %Y";
            $divabonnement = 12;
            break;
        default:
            die("Periode erronée, valeurs possibles: [8jours|8semaines|8mois|1an] !");
            break;
    }

    $tab_optarif = getOPTARIF(true);
    $optarif = $tab_optarif["OPTARIF"];
    $isousc = $tab_optarif["ISOUSC"];

    $query = queries::queryHistory($timestampdebut, $timestampfin, $dateformatsql, $optarif);

    $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        exit();
    }
    $nbenreg = $result->num_rows;

    if ($nbenreg > 0) {
        $kwhprec = array();
        $kwhprec_detail = array();
        $date_deb=0; // date du 1er enregistrement
        $date_fin=time();

        $row = $result->fetch_array();
        $optarif = $teleinfo["OPTARIF"][$row["OPTARIF"]];
        //$optarifStr = $teleinfo["LIBELLES"]["OPTARIF"][$teleinfo["OPTARIF"][$optarif]];
        $optarifStr = $teleinfo["LIBELLES"]["OPTARIF"][$optarif];
        $ptec = $teleinfo["PTEC"][$row["PTEC"]];
        $ptecStr = $teleinfo["LIBELLES"]["PTEC"][$ptec];
        $annotations[]=$row["annotations"];
        // On initialise à vide
        // Cas si les périodes sont "nulles", on n'aura pas d'initialisation des tableaux
        foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
            $kwh[$ptec] = array();
            $kwhp[$ptec] = array();
        }
        $categories = array();
        $timestp = array();
        $timestpp = array();
        $annotations =  array();
        // Calcul des consommations
        $result->data_seek(0); // Revient au début (car on a déjà lu un enreg)
        while ($row = $result->fetch_array()) {
            $ts = intval($row["TIMESTAMP"]);
                // Période courante
                if ($date_deb==0) {
                    $date_deb = $row["TIMESTAMP"];
                    //$date_deb = strtotime($row["REC_DATE"]);
                }
                // Ajout les éléments actuels à chaque tableau
                $categories[] = $row["PERIODE"];
                $timestp[] = $row["TIMESTAMP"];
                foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
                    $kwh[$ptec][] = floatval(isset($row[$ptec]) ? $row[$ptec] : 0);
                }
                $annotations[]=$row["annotations"];
           // }
        }
        // On vérifie la durée de la période actuelle
        if (count($kwh) < $duree) {
            // pad avec une valeur négative, pour ajouter en début de tableau
            $timestp = array_pad ($timestp, -$duree, null);
            $categories = array_pad ($categories, -$duree, null);
            foreach($kwh as &$current) {
                $current = array_pad ($current, -$duree, null);
            }
        }
        $result->free();
        $date_deb_UTC=$date_deb*1000;
        $datetext = date("d/m G:i", $date_deb);
        // Calcul des coûts
        $tab_prix = getTarifsEDF($optarif, $isousc);

        foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
            $mnt["KWH"][$ptec] = 0;
            $mntp["KWH"][$ptec] = 0;
        }
        $i = 0;
        $rounds = max(count(reset($kwh)), count(reset($kwhp)));
        while ($i < $rounds) {
            // Puriste, on sépare les traitements des périodes courante et précédente.
            // - Si l'option tarifaire évolue (pas encore pris en charge)
            // - Si les taxes évoluent
            // Période courante
            // On recherche la base tarifaire pour cette période (date)
            $cur_prix = getTarifs($tab_prix, $timestp[$i]);

            foreach($cur_prix["TAXES_C"] as $tkey => $tval) {
                $mnt["TAXES"][$tkey][$i] = 0; // Init à zéro
            }
            $mnt["TOTAL"][$i] = 0;
            $conso = false;

            foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
                // TaxesC
                foreach($cur_prix["TAXES_C"] as $tkey => $tval) {
                    $mnt["TAXES"][$tkey][$i] += $kwh[$ptec][$i] * $cur_prix["TAXES_C"][$tkey];
                    $mnt["TOTAL"][$i] += $kwh[$ptec][$i] * $cur_prix["TAXES_C"][$tkey];
                }
                // Consommation
                $mnt["TARIFS"][$ptec][$i] = $kwh[$ptec][$i] * $cur_prix["TARIFS"][$ptec];
                $mnt["TOTAL"][$i] += $kwh[$ptec][$i] * $cur_prix["TARIFS"][$ptec];
                $mnt["KWH"][$ptec] += $kwh[$ptec][$i];
                $conso = ($conso or ($kwh[$ptec][$i]!=0));
            }
            // TaxesA
            foreach($cur_prix["TAXES_A"] as $tkey => $tval) {
                $mnt["TAXES"][$tkey][$i] = $cur_prix["TAXES_A"][$tkey] / $divabonnement;
                $mnt["TOTAL"][$i] += $cur_prix["TAXES_A"][$tkey] / $divabonnement;
            }
            // Abonnement
            $mnt["ABONNEMENTS"][$i] = $conso * $cur_prix["ABONNEMENTS"][$optarif] / $divabonnement;
            $mnt["TOTAL"][$i] += $conso * $cur_prix["ABONNEMENTS"][$optarif] / $divabonnement;

            $i++;
        }

        // Totaux période courante
        foreach($mnt["TAXES"] as $tkey => $tval) {
            $total_mnt["TAXES"][$tkey] = array_sum($mnt["TAXES"][$tkey]);
        }
        foreach($mnt["TARIFS"] as $tkey => $tval) {
            $total_mnt["TARIFS"][$tkey] = array_sum($mnt["TARIFS"][$tkey]);
        }
        $total_mnt["ABONNEMENTS"] = array_sum($mnt["ABONNEMENTS"]);
        $total_mnt["TOTAL"] = array_sum($mnt["TOTAL"]);
        $total_mnt["KWH"] = array_sum($mnt["KWH"]);

        // Subtitle pour la période courante
        $subtitle = "";
        if ($total_mnt["TOTAL"] != 0) { // A priori, toujours vrai !
            $subtitle = $subtitle."<b>Coût sur la période</b> ".round($total_mnt["TOTAL"],2)." Euro (".$total_mnt["KWH"]." KWh)<br />";
            $subtitle = $subtitle."(Abonnement : ".round($total_mnt["ABONNEMENTS"],2);
            $subtitle = $subtitle." + Taxes (" . implode(", ", array_keys($total_mnt["TAXES"])) . ") : ".round(array_sum($total_mnt["TAXES"]),2);
            foreach($total_mnt["TARIFS"] as $ptec => $val) {
                if ($total_mnt["TARIFS"][$ptec] != 0) {
                    $subtitle = $subtitle." + ".$ptec." : ".round($total_mnt["TARIFS"][$ptec],2);
                }
            }
            $subtitle = $subtitle.")";
            if ((count($total_mnt["TARIFS"]) > 1) && ($total_mnt["KWH"] > 0)) {
                $subtitle = $subtitle."<br /><b>Total KWh</b> ";
                $prefix = "";
                foreach($mnt["KWH"] as $ptec => $val) {
                    if ($mnt["KWH"][$ptec] != 0) {
                        $subtitle = $subtitle.$prefix.$ptec." : ".$mnt["KWH"][$ptec];
                        if ($prefix=="") {
                            $prefix = " + ";
                        }
                    }
                }
            }
        }

        // Conserve les séries nécessaires
        $series = array_intersect_key($teleinfo["LIBELLES"]["PTEC"], array_flip($teleinfo["PERIODES"][$optarif]));

        $history = array(
            'show3D' => $graphConf["show3D"],
            'title' => "Consommation sur $xlabel",
            'subtitle' => $subtitle,
            'optarif' => array($optarif => $optarifStr),
            'ptec' => array($ptec => $ptecStr),
            'duree' => $duree,
            'periode' => $periode,
            'debut' => $timestampfin*1000,
            'series' => $series,
            'prix' => $mnt,
            'prix_tot' => $total_mnt,
            'categories' => $categories,
            'ANNOTATIONS' => $annotations
        );

        // Ajoute les séries
        foreach($teleinfo["PERIODES"][$optarif] as $ptec) {
            $history[$ptec."_name"] = $series[$ptec];
            $history[$ptec."_color"] = $teleinfo["COULEURS"][$ptec];
            $history[$ptec."_data"] = $kwh[$ptec];
            $history[$ptec."_type"] = $graphConf["typeSerie"];
         }

        return $history;
    } else {
        $result->free();
        return null;
    }
}

function set_annot($annot,$t)
 {
    global $db_connect, $config_table;
    global $teleinfo;
    global $config;
    global $mysqli;
     $query = "UPDATE `" .$db_connect['table'] ."` SET `".$config_table["table"]["ANNOTATIONS"] ."` = \"" .$annot ."\" WHERE `" .$db_connect['table'] ."`.`DATE` = " .$t;
     $result=$mysqli->query($query);
    if (!$result) {
        printf("<b>Erreur</b> dans la requête <b>" . $query . "</b> : "  . $mysqli->error . " !<br>");
        return false;   //pas réussi à gérer le retour false ...
    }
    return true;
}

function main() {
    global $db_connect;
    global $mysqli;

    $query = isset($_GET['query'])?$_GET['query']:"daily";

    if (isset($query)) {
        $mysqli = new mysqli($db_connect['serveur'], $db_connect['login'], $db_connect['pass'], $db_connect['base']);
        if (mysqli_connect_errno()) {
            printf("Erreur de connexion au serveur MySql : %s\n", mysqli_connect_error());
            exit();
        }
        if (!$mysqli->set_charset("utf8")) {
            printf("Erreur lors du chargement du jeu de caractères utf8 : %s\n", $mysqli->error);
            exit();
        }
        $mysqli->query("SET lc_time_names = 'fr_FR'");  // Pour afficher date en français dans MySql.

        switch ($query) {
            case "instantly":
                $data=instantly();
                break;
            case "daily":
                $data=daily();
                break;
            case "history":
                $data=history();
                break;
            case "get_fonctionnel":
                $data=get_fonctionnel();
                break;
            case "get_vmc":
                $data=get_vmc();
                break; 
           case "setannot":
                if(!(isset($_GET['annot'])))
                {
                    $data= false;
                    break;      
                }  
                if(!(isset($_GET['t'])))
                {
                    $data= false;
                    break;    
                }  
                $annot =$_GET['annot'];
                $t = $_GET['t'];
                $data=set_annot($annot,$t);
                break;                               
            default:
                break;
        };
        $mysqli->close();

        if ($data !== null ) { 
            echo json_encode($data);
        }
    }
}

main();
?>
